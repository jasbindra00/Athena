#ifndef ANIMATION_BASE_H
#define ANIMATION_BASE_H
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>


using FrameNumber = unsigned int; //frame label for flexible frame sequencing
using FrameDetails = std::pair<float, sf::IntRect>; //{frametime,texture rect from atlasmap}
using FrameMap = std::unordered_map<FrameNumber, FrameDetails>;
class Animation_Base //class for keeping the animation timings and texture rect croppings (can be used with sprite).
{
protected:
	mutable bool looping;
	mutable bool playing;
	FrameMap animationframes;
	FrameNumber activeframe;
	sf::Clock timer; //for timing the duration of each frame
	inline void IncrementFrame() {
		if (!looping && activeframe == animationframes.size() - 1) return; //stay at last frame if not looping
		++activeframe;
		if (looping && activeframe == animationframes.size()) activeframe = 0; //wrap to first frame after last frame executed.
	}
public:
	Animation_Base() :looping(true), playing(true), activeframe(0) {
	}
	inline void AddFrame(const FrameDetails& framedetails) { //adds frame to the end of the current sequence
		animationframes[animationframes.size()] = framedetails;
	}
	void Update() {
		if (animationframes.empty()) return;
		auto& currentframe = animationframes[activeframe];
		if (playing && timer.getElapsedTime().asSeconds() > currentframe.first) { //if the current frame has exceed its time limit
			IncrementFrame();//change the frame
			timer.restart();
		}
	}
	auto GetActiveFrame() { //used by the animation component to perform the cropping of the sprite.
		return animationframes[activeframe];
	}
	void ClearFrames() { animationframes.clear(); }
	void StopAnimation() const { playing = false;}
	void PlayAnimation() const { playing = true; }
	void SetLooping(const bool& input) const { looping = input; }

	bool IsPlaying() const { return playing; }
	bool IsLooping() const { return looping; }

};



#endif