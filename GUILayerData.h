#ifndef GUILAYERDATA_H
#define GUILAYERDATA_H
#include <SFML/Graphics.hpp>
#include "GUIInterface.h"
#include <array>
#include "GUILayerData.h"
namespace GUILayerData {
	using GUIData::GUILayerType;
	class GUILayer{
	private:
		GUILayerType layertype;
		sf::RenderTexture layer;
		sf::Sprite layersprite;
		mutable bool pendinglayerredraw{ true };
	public:
		GUILayer(const sf::Vector2f& eltsize, const GUILayerType& type):layertype(type) {
			InitLayer(eltsize);
		}
		GUILayer(const GUILayerType& type):layertype(type) {

		}

		void InitLayer(const sf::Vector2f& eltsize) {
			layer.clear(sf::Color::Color(255, 255, 255, 255));
			layer.create(eltsize.x, eltsize.y);
			layersprite.setTexture(layer.getTexture());
		}
		void Draw(sf::RenderTarget& target, const bool& toparent) {
			target.draw(layersprite);
		}
		void RefreshLayer(GUIVisual& visual, const GUIElements& elements) {
			layer.clear(sf::Color::Color(0, 0, 0, 0));
			if(layertype == GUILayerType::BACKGROUND) visual.Draw(layer, true);
			for (auto& elt : elements) {
				if (elt.second->GetLayerType() == layertype) {
					if (elt.second->IsHidden()) continue;
					//ensure that all of the children have been drawn onto the interface.
					  //we need to call UpdateLayer on the interface.
					elt.second->Draw(layer, true);
					//draw it to the layer.
				}
				//if its not part of the current layer, then do we still draw in? no. the other layers will handle it since we are in the middle of a loop going through all layers.
			}
			layer.display();
			pendinglayerredraw = false;			
		}
		void SetSize(const sf::Vector2f& size) {
			layer.create(size.x, size.y);
			layer.clear(sf::Color::Color(0, 0, 0, 0));
			pendinglayerredraw = true;
		}
		void Draw(const sf::Drawable& drawable) {
			layer.draw(drawable);
			pendinglayerredraw = true;
		}
		void SetPosition(const sf::Vector2f& position) {
			layersprite.setPosition(position);
			pendinglayerredraw = true;
		}
		sf::RenderTarget& GetTarget() { return layer; }
		const bool& PendingLayerRedraw() const { return pendinglayerredraw; }
		void QueueLayerRedraw() const { pendinglayerredraw = true;}
	};
using GUIFormattingData::GUIVisual;
using GUIData::GUILayerType;

	using GUILayerPtr = std::unique_ptr<GUILayer>;
	class GUILayers {
	private:
		sf::Vector2f interfacesize;
		sf::Vector2f interfaceposition;
		bool pendingpositionapply{ true };
		bool pendingsizeapply{ true };
		bool pendingparentredraw{ true };
		std::array<GUILayerPtr, 3> layers;
		GUILayerPtr& GetLayer(const GUILayerType& LAYER) { return layers[static_cast<int>(LAYER)];}
	public:
		GUILayers() {
			OnCreate();
		}
		GUILayers(const sf::Vector2f& eltsize) {
			OnCreate(eltsize);
		}
		void OnCreate(const sf::Vector2f& eltsize = {}) {
			if (eltsize != sf::Vector2f{}) for (int i = 0; i < 3; ++i) layers[i] = std::make_unique<GUILayer>(eltsize,static_cast<GUILayerType>(i));
			else for (int i = 0; i < 3; ++i) layers[i] = std::make_unique<GUILayer>(static_cast<GUILayerType>(i));
		}
		void RefreshChanges(GUIVisual& visual, const GUIElements& elements) {
			for (int layernum = 0; layernum < layers.size(); ++layernum) {
				auto& layer = layers[layernum];
				if (pendingsizeapply) { layer->SetSize(interfacesize); }
				if (pendingpositionapply) { layer->SetPosition(interfaceposition); }
				//loop through all elements, check if they are requesting a parent redraw.
				bool child_redraw_request{ false };
				for (auto& elt : elements) {
					if (elt.second->PendingParentRedraw()) {
						child_redraw_request = true;
						break;
					}
				}
				if (layer->PendingLayerRedraw() || child_redraw_request) { //only refresh changed layers.
					layer->RefreshLayer(visual, elements);
				}
			}
			pendingsizeapply = false;
			pendingpositionapply = false;
		}
		void Rasterize(sf::RenderTarget& target, const bool& toparent) {
			for (auto& layer : layers) layer->Draw(target, toparent);
			if (toparent) pendingparentredraw = false;
		}
		void QueueSize(const sf::Vector2f& size) {
			interfacesize = size;
			pendingsizeapply = true;
			pendingparentredraw = true;
		}
		void QueuePosition(const sf::Vector2f& position) {
			interfaceposition = position;
			pendingpositionapply = true;
			pendingparentredraw = true;
		}
		void QueueLayerRedraw(const GUILayerType& layertype) { 
			layers.at(static_cast<int>(layertype))->QueueLayerRedraw();
			pendingparentredraw = true;
		}
		void QueueParentRedraw() {
			pendingparentredraw = true;
		}
		const bool& PendingParentRedraw() const { return pendingparentredraw; }
		void DrawToLayer(const GUILayerType& layer, const sf::Drawable& drawable) {
			GetLayer(layer)->Draw(drawable); 
			pendingparentredraw = true;
		}
		sf::RenderTarget& GetLayerTarget(const GUILayerType& layer) { return layers.at(static_cast<int>(layer))->GetTarget(); }
		const sf::Vector2f& GetPosition() const { return interfaceposition; }
	};
}
#endif