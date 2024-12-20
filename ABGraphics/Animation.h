#pragma once

struct Keyframe
{
	float TimePos = 0.f;
	Vector3 Translation = { 0.f, 0.f, 0.f };
	Vector3 Scale = { 1.f, 1.f, 1.f };
	Quaternion Rotation;
};

class Keyframes
{
public:
	Keyframes(const string& name, const vector<Keyframe>& keyframes);
	Keyframes() = default;

	static Matrix CreateMatrix(const Keyframe& keyframe);
	static Keyframe Interpolate(const Keyframe& lhs, const Keyframe& rhs, float lhsWeight);

	Keyframe Interpolate(float timePos) const;

	size_t GetKeyframeIndex(float timePos) const;
	const string& GetName() const { return mName; }
	float GetStartTime() const { return mKeyframes.front().TimePos; }
	float GetEndTime() const { return mKeyframes.back().TimePos; }

private:
	string mName;
	std::vector<Keyframe> mKeyframes;
};

class AnimClip
{
public:
	AnimClip(aiAnimation* animation);
	~AnimClip() = default;

	bool TryGetkeyframes(string boneName, const Keyframes** outKeyframes) const;
	const Keyframes& Getkeyframes(string boneName) const;

	const string& GetName() const { return mName; }
	float GetDuration() const { return mDuration; }
	const map<string, std::shared_ptr<Keyframes>>& GetBoneKeyframes() const { return mBoneKeyframes; }
	size_t GetFrameIndex(float timePos, bool bIsRecursive) const;

private:
	std::string mName;
	float mDuration;
	size_t mFrameSize;
	float mTimePerFrame;
	std::map<std::string, std::shared_ptr<Keyframes>> mBoneKeyframes;
};

class AnimResource
{
public:
	AnimResource(const std::string& path);
	~AnimResource() = default;

	const std::string GetPath() const { return mPath; }
	const string& GetFileName() const { return mName; }
	const AnimClip& GetAnimClip(string name) const;
	const AnimClip& GetAnimClip(size_t index) const;
	const map<string, AnimClip> GetAnimClips() const { return mAnimClips; }
	const set<std::string> GetAnimClipNames() const;

private:
	std::string mPath;
	std::string mName;
	std::map<std::string, AnimClip> mAnimClips;
};

// 0 ~ 1로 대응된 UV의 변환으로 사용 가능하도록 구성
struct Sprite
{
	Vector2 TexTranslate;
	Vector2 TexSize;
	float TimePos;
};

struct SpriteClip
{
	std::vector<Sprite> Sprites;
	float Duraition;
};

class SpriteAnimation
{
public:
	SpriteAnimation(Vector2 textureSize, Vector2 spriteSize, float duration);
	SpriteAnimation(Vector2 textureSize, Vector2 spriteSize, size_t spriteCountY, const std::vector<size_t>& spriteCountX, const std::vector<std::vector<float>>& durations);
	~SpriteAnimation() = default;

	const Sprite& GetSprite(size_t x, size_t y, bool bIsRecursive) const;
	const Sprite& GetSprite(size_t y, float timePos, bool bIsRecursive) const;

private:
	std::vector<SpriteClip> mSpriteClips;
};