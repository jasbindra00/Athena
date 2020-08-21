#ifndef GUIINTERFACELAYERS_H
#define GUIINTERFACELAYERS_H
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include "GUIFormatting.h"




namespace GUILayerData {
	struct Layer : private sf::RenderTexture{
		friend class GUIInterfaceLayers;
	private:
		sf::Sprite layersprite;
		bool pendingredraw{ false };
	public:
		Layer() {
			layersprite.setTexture(getTexture());
		}
		using sf::RenderTexture::create;
		using sf::RenderTexture::clear;
		using sf::RenderTexture::draw;
		void DrawLayer(sf::RenderTarget& target) const {
			target.draw(layersprite);
		}
	};
	using GUIData::GUILayerType;
	class GUIInterfaceLayers {
		friend class GUIInterface;
	private:
		mutable bool pendingparentredraw;
		std::array<std::unique_ptr<Layer>, 3> layers;
		GUIVisual* interfacevisual;
	public:
		void QueueLayerRedraw(const GUILayerType& layertype) { layers[static_cast<int>(layertype)]->pendingredraw = true; }
		void RedrawLayer(const GUILayerType& layertype, GUIElements& elements, sf::RenderTarget& target, GUIVisual* visual) {
			auto& layer = layers.at(static_cast<int>(layertype));
			layer->clear(sf::Color::Color(255, 255, 255, 0));
			if (layertype == GUILayerType::BACKGROUND) visual->Draw(target);
			for (auto& elt : elements) {
				if (elt.second->IsHidden()) continue;
				if (elt.second->GetLayerType() == layertype) {
					elt.second->QueuedRedraw()
					elt.second->Draw(target);
					elt.second->ResetRedraw();
				}	
			}
			layer.display();
			layers[static_cast<int>(layert)].pendingredraw = false;
			parentredraw = true;
		}
		GUIInterfaceLayers() {
		}
		void InitLayers(const sf::Vector2f& eltsize) {
			for (auto& layer : layers) {
				layer.SetSize(eltsize);
			}
		}
		void Render(sf::RenderTarget& target) { //part of another interface
			std::for_each(layers.begin(), layers.end(), [&target](const Layer& layer) {
				layer.DrawLayer(target);
				});
		}
		template<GUILayerType LayerType>
		const bool& QueuedRedraw() const { return layers.at(static_cast<int>(LayerType)).pendingredraw; }
	};
}


#endif