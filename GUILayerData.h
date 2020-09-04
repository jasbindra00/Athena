#ifndef GUILAYERDATA_H
#define GUILAYERDATA_H
#include <SFML/Graphics.hpp>
#include "GUIInterface.h"
#include <array>
#include "GUILayerData.h"
#include <functional>



namespace GUILayerData {
	using GUIData::GUILayerType;

	using LayerRefreshCallable = std::function<void(sf::RenderTarget&)>;
	class RenderLayer {
	private:
		std::vector<sf::Drawable*> user_drawables; //ADD FUNCTIONALITY FOR FINER CONTROL OF THESE

		sf::RenderTexture layer;
		sf::Sprite layer_sprite;
		
		mutable bool pending_layer_redraw{ true };
		GUILayerType layer_type;

		mutable sf::Color clear_color{ 0,0,0,0 };
	public:
		RenderLayer(const sf::Vector2f& eltsize, const GUILayerType& type) :layer_type(type) {
			InitLayer(eltsize);
		}
		RenderLayer(const GUILayerType& type) :layer_type(type) {

		}
		void Rasterize(sf::RenderTarget& target, const bool& to_parent) {
			target.draw(layer_sprite);
		}
		void RefreshLayer(const LayerRefreshCallable& callable) {
			layer.clear(clear_color);
			callable(static_cast<sf::RenderTarget&>(layer));
			for (auto& drawable : user_drawables) {
				layer.draw(*drawable);
			}
			layer.display();
			user_drawables.clear();
			pending_layer_redraw = false;
		}
		void InitLayer(const sf::Vector2f& elt_size) {
			layer.clear(clear_color);
			layer.create(elt_size.x, elt_size.y);
			layer_sprite.setTexture(layer.getTexture());
		}
		void SetSize(const sf::Vector2f& size) {
			layer.create(size.x, size.y);
			layer.clear(clear_color);
			pending_layer_redraw = true;
		}
		void CycleDrawables(const std::vector<sf::Drawable*>& drawables) {
			user_drawables = drawables;
			pending_layer_redraw = true;
		}
		void SetView(sf::View&& v) {
			layer.setView(std::move(v));
			pending_layer_redraw = true;
		}
		void Zoom(const float& factor) {
			auto x = layer.getView();
			x.zoom(factor);
			layer.setView(std::move(x));
		}
		void SetPosition(const sf::Vector2f& position) {
			layer_sprite.setPosition(position);
			pending_layer_redraw = true;
		}
		const sf::Vector2f& GetPosition() const {
			return layer_sprite.getPosition();
		}
		sf::RenderTarget& GetTarget() { return layer; } //NOT SAFE. USER COULD DRAW SOMETHING AND DISREGARD THE SYSTEM.
		void SetClearColor(sf::Color&& c) const {
			clear_color = std::move(c);
		}
		const bool& PendingLayerRedraw() const { return pending_layer_redraw; }
		void QueueLayerRedraw() const { pending_layer_redraw = true; }
		const GUILayerType& GetLayerType() { return layer_type; }
		const sf::Sprite& GetLayerSprite() const { return layer_sprite; }
	};

using GUIFormattingData::GUIVisual;
using GUIData::GUILayerType;

	using GUILayerPtr = std::unique_ptr<RenderLayer>;
	class GUILayers {
	private:
		sf::Vector2f interfacesize;
		sf::Vector2f interfaceposition;
		bool pendingpositionapply{ true };
		bool pendingsizeapply{ true };
		bool pendingparentredraw{ true };
		std::array<GUILayerPtr, 3> layers;
		GUILayerPtr& GetLayer(const GUILayerType& LAYER) { return layers[static_cast<int>(LAYER)];}
		void RefreshLayerCallable(GUIVisual& visual, const GUIElements& elements, RenderLayer* layer, sf::RenderTarget& layer_target) {
			const auto& layer_type = layer->GetLayerType();
			if (layer_type == GUILayerType::BACKGROUND) visual.Draw(layer->GetTarget(), true);
			for (auto& elt : elements) {
				if (elt.second->GetLayerType() == layer_type) {
					if (elt.second->IsHidden()) continue;
					//ensure that all of the children have been drawn onto the interface.
					  //we need to call UpdateLayer on the interface.
					elt.second->Draw(layer_target, true);
					//draw it to the layer.
				}
			}
		}

	public:
		GUILayers() {
			OnCreate();
		}
		GUILayers(const sf::Vector2f& eltsize) {
			OnCreate(eltsize);
		}
		void OnCreate(const sf::Vector2f& eltsize = {}) {
			if (eltsize != sf::Vector2f{}) {
				for (int i = 0; i < 3; ++i) {
					layers[i] = std::make_unique<RenderLayer>(eltsize, static_cast<GUILayerType>(i));
				}
			}
			else {
				for (int i = 0; i < 3; ++i) {
					layers[i] = std::make_unique<RenderLayer>(static_cast<GUILayerType>(i));
				}
			}
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
						child_redraw_request = true; //Child redraw is reset on the actual draw cycle.
						break;
					}
				}
				if (layer->PendingLayerRedraw() || child_redraw_request) {
					layer->RefreshLayer([&visual, &elements, &layer, this](sf::RenderTarget& layer_target) {this->RefreshLayerCallable(visual, elements, layer.get(), layer_target); });
				}
			}
			pendingsizeapply = false;
			pendingpositionapply = false;
		}
		void SetLayerView(const GUILayerType& layer, sf::View&& v) {
			GetLayer(layer)->SetView(std::move(v));
			pendingparentredraw = true;
		}
		void Rasterize(sf::RenderTarget& target, const bool& toparent) {
			for (auto& layer : layers) layer->Rasterize(target, toparent);
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
		void DrawToLayer(const GUILayerType& layer, const std::vector<sf::Drawable*>& drawables) {//CHANGE THIS FOR FINER USER - DRAWABLE CONTROL.
			GetLayer(layer)->CycleDrawables(drawables); 
			pendingparentredraw = true;
		}

		sf::RenderTarget& GetLayerTarget(const GUILayerType& layer) { return layers.at(static_cast<int>(layer))->GetTarget(); }
		const sf::Vector2f& GetPosition() const { return interfaceposition; }
	};
}
#endif