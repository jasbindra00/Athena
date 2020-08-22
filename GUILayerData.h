#ifndef GUILAYERDATA_H
#define GUILAYERDATA_H
#include <SFML/Graphics.hpp>
#include "GUIInterface.h"
#include <array>
namespace GUILayerData {
	using GUIData::GUILayerType;
	class GUILayer{
	private:
		GUILayerType layertype;
		sf::RenderTexture layer;
		sf::Sprite layersprite;
		mutable bool pendinglayerredraw{ true };
		mutable bool pendingparentredraw{ true };
	public:
		GUILayer(const sf::Vector2f& eltsize, const GUILayerType& type):layertype(type) {
			layer.clear(sf::Color::Color(255, 255, 255, 255));
			layer.create(eltsize.x, eltsize.y);
		}
		//render to parent
		void Render(sf::RenderTarget& target, const bool& toparent) {
			target.draw(layersprite);
			if (toparent) pendingparentredraw = false;
		}
		//this layer is a parent
		void RedrawLayer(GUIVisual& visual, const GUIElements& elements) { 
			layer.clear(sf::Color::Color(255, 0, 0, 255));
			visual.Render(layer, true);
			if (layertype == GUILayerType::BACKGROUND) visual.Render(layer, true);//drawn to parent.
			for (auto& element : elements) {
				if (element.second->IsHidden()) continue;
				if (element.second->GetLayerType() == layertype) { //then it must be a content elt
					if (dynamic_cast<GUIInterface*>(element.second.get()) && layertype != GUILayerType::CONTENT) continue; //interfaces are only drawn to our content layer.
					element.second->Render(layer,true);
				}
			}
			layer.display();
			pendinglayerredraw = false;
			pendingparentredraw = true;
		}
		void SetSize(const sf::Vector2f& size) {
			layer.create(size.x, size.y);
			pendinglayerredraw = true;
			pendingparentredraw = true;
		}
		void Draw(const sf::Drawable& drawable) {
			layer.draw(drawable);
			pendinglayerredraw = true;
			pendingparentredraw = true;
		}
		void SetPosition(const sf::Vector2f& position) {
			layersprite.setPosition(position);
			pendingparentredraw = true;
		}
		const bool& PendingLayerRedraw() const { return pendinglayerredraw; }
		const bool& PendingParentRedraw() const { return pendingparentredraw; }
		void QueueLayerRedraw() const { pendingparentredraw = true; pendingparentredraw = true; }
		void QueueParentRedraw() const { pendingparentredraw = true; }
	};
using GUIFormattingData::GUIVisual;
using GUIData::GUILayerType;

	using GUILayerPtr = std::unique_ptr<GUILayer>;
	class GUILayers {
	private:
		sf::Vector2f interfacesize;
		sf::Vector2f interfaceposition;
		bool pendingpositionapply{ false };
		bool pendingsizeapply{ false };

		std::array<GUILayerPtr, 3> layers;
		GUILayerPtr& GetLayer(const GUILayerType& LAYER) {
			return layers[static_cast<int>(LAYER)];
		}
	public:
		GUILayers(const sf::Vector2f& eltsize) {
			for (int i = 0; i < 3; ++i) layers[i] = std::make_unique<GUILayer>(eltsize, static_cast<GUILayerType>(i));
		}
		void Update(GUIVisual& visual, const GUIElements& elements) {
			for (int layernum = 0; layernum < layers.size(); ++layernum) {
				auto& layer = layers[layernum];
				if (pendingsizeapply) { layer->SetSize(interfacesize); pendingsizeapply = false; }
				if (pendingpositionapply) { layer->SetPosition(interfaceposition); pendingpositionapply = false; }
				if (layer->PendingLayerRedraw()) layer->RedrawLayer(visual, elements);
				}
			}
		
		void Render(sf::RenderTarget& target, const bool& toparent) {
			for (auto& layer : layers) layer->Render(target, toparent);
		}
		std::array<bool, 3> PendingParentRedraw() const {
			std::array<bool, 3> res;
			for (int i = 0; i < 3; ++i) res[i] = layers[i]->PendingParentRedraw();
			return res;
		}
		void QueueSize(const sf::Vector2f& size) {
			interfacesize = size;
			pendingsizeapply = true;
		}
		void QueuePosition(const sf::Vector2f& position) {
			interfaceposition = position;
			pendingpositionapply = true;
		}
		void QueueLayerRedraw(const GUILayerType& layertype) { layers.at(static_cast<int>(layertype))->QueueLayerRedraw();}
		void DrawToLayer(const GUILayerType& layer, const sf::Drawable& drawable) { GetLayer(layer)->Draw(drawable); }
	
	};
}
#endif