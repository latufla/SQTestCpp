#include "RenderEngine.h"

#include <SFML/OpenGL.hpp>
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>

using std::shared_ptr;
using std::numeric_limits;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::cross;
using glm::dot;
using glm::inverse;
using glm::normalize;
using glm::ortho;

namespace sqr{

	RenderEngine::RenderEngine(shared_ptr<ILoader> assetLoader) 
		:assetLoader(assetLoader) {

		window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1024, 768), "SFML");
	}
	
	RenderEngine::~RenderEngine() {

	}

	void RenderEngine::addObject(uint32_t id, shared_ptr<IView> object) {
		idToObject.emplace(id, object);
		// we should load geometry to gpu here
	}

	void RenderEngine::removeObject(uint32_t id) {
		idToObject.erase(id);
		// we should unload geometry from gpu here
	}

	shared_ptr<IView> RenderEngine::getObjectBy(uint32_t id) {
		return idToObject.at(id);
	}

	bool RenderEngine::doStep(double step) {
		if(!window->isOpen())
			return false;

		sf::Event event;
		while(window->pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window->close();
		}
		
		window->clear(sf::Color::Black);
		
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_LIGHTING); 

		auto wSize = window->getSize();
		glViewport(0, 0, wSize.x, wSize.y);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		
		float ratio = (float)wSize.x / (float)wSize.y;
		mat4 projection = ortho(-ratio, ratio, -1.f, 1.f);

		for(auto& i : idToObject) {
			auto view = i.second;
			auto key = view->getModelKeyName();
			auto model = assetLoader->getModel3dBy(key);
			auto meshes = model->getMeshes();
			for(auto j : meshes) {
				auto vertices = j->getVertices();				
				auto color = view->getColor(); 
				
				std::vector<float> vxs;
				std::vector<float> colors;
				for(auto k : vertices) {
					auto pos = k->getPosition();
					vxs.push_back(pos[0]);
					vxs.push_back(pos[1]);
					vxs.push_back(pos[2]);
					colors.push_back(color[0]);
					colors.push_back(color[1]);
					colors.push_back(color[2]);
					colors.push_back(color[3]);
				}
				
				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf(&projection[0][0]);			
				
				glMatrixMode(GL_MODELVIEW);
				auto transform = view->getTransform();
				glLoadMatrixf(&transform[0][0]);

				glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), &vxs[0]);
				glColorPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), &colors[0]);
				
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
		}
		window->display();
				
		calcMouseOverObject(projection);

		return true;
	}

	bool RenderEngine::getMouseLeftDown() {
		return sf::Mouse::isButtonPressed(sf::Mouse::Left);
	}

	int32_t RenderEngine::getMouseOver() {
		return mouseOver;
	}

	void RenderEngine::calcMouseOverObject(glm::mat4& projection) {
		mouseOver = -1;

		auto mousePos = sf::Mouse::getPosition(*window.get());
		auto wPos = window->getPosition();
		auto wSize = window->getSize();
		
		vec2 mouseNdc = vec2{mousePos.x, mousePos.y} / vec2{wSize.x, wSize.y} * 2.0f - 1.0f;
		mouseNdc.y = -mouseNdc.y;

		mat4 toWorld = inverse(projection);

		vec4 mouseWorld = toWorld * vec4(mouseNdc.x, mouseNdc.y, 0.0f, 1.0f);
		vec3 mouse{mouseWorld};

		for(auto& i : idToObject) {
			auto id = i.first;
			auto view = i.second;
			auto key = view->getModelKeyName();
			auto model = assetLoader->getModel3dBy(key);
			auto meshes = model->getMeshes();
			for(auto j : meshes) {
				auto vertices = j->getVertices();
				for(uint8_t k = 0; k < vertices.size(); k += 3){
					vec3 a = vertices[k]->getPosition();					
					a = vec3{view->getTransform() * vec4{a, 1.0f}};

					vec3 b = vertices[k + 1]->getPosition();
					b = vec3{view->getTransform() * vec4{b, 1.0f}};

					vec3 c = vertices[k + 2]->getPosition();
					c = vec3{view->getTransform() * vec4{c, 1.0f}};

					if(isInsideTrianle(mouse, a, b, c)) {
						mouseOver = id;
						return;
					}
				}
			}
		}
	}

	bool RenderEngine::isInsideTrianle(vec3& p, vec3& a, vec3& b, vec3& c) {
		vec3 v0 = c - a;
		vec3 v1 = b - a;
		vec3 v2 = p - a;

		float dot00 = dot(v0, v0);
		float dot01 = dot(v0, v1);
		float dot02 = dot(v0, v2);
		float dot11 = dot(v1, v1);
		float dot12 = dot(v1, v2);

		float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
	}
}
