#ifndef COMPONENT_ANIMATION_H
#define COMPONENT_ANIMATION_H
#include "Component_Base.h"
#include "Animation_Base.h"
#include "Utility.h"
using BaseAnimPtr = std::unique_ptr<Animation_Base>;
class Component_Animation : public Component_Base {//component for holding a map of animations which are individually responsible for their timings.
protected:
	std::unordered_map<std::string, BaseAnimPtr> animations;
	Animation_Base* currentanimation{ nullptr };
public:
	Component_Animation() : Component_Base(ComponentType::ANIMATION) {
	}
	Animation_Base* GetCurrentAnimation() { return currentanimation; }
	void AddAnimation(const std::string& animationname, BaseAnimPtr animobj) {
		animations[animationname] = std::move(animobj);
		ChangeAnimation(animationname);
	}
	void AddFrame(const std::string& animname, const FrameDetails& frame) {
		auto foundanim = animations.find(animname);
		if (foundanim == animations.end()) {
			AddAnimation(animname, std::make_unique<Animation_Base>());
		}
		animations[animname]->AddFrame(frame);
	}
	virtual void ReadIn(std::stringstream& stream) override {

	}
	void ChangeAnimation(const std::string& animname) {
		auto foundanim = animations.find(animname);
		if (foundanim == animations.end()) {
			Utility::log.Log(LOCATION::COMPONENT_ANIMATION, LOGTYPE::ERROR, "ChangeAnimation()", "Could not find animation of name " + animname);
			return;
		}
		currentanimation = foundanim->second.get();
	}
	virtual ~Component_Animation() {

	}
};

	

#endif
