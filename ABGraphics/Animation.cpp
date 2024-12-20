#include "pch.h"

#include "Animation.h"

Keyframes::Keyframes(const string& name, const vector<Keyframe>& keyframes)
	: mName(name)
	, mKeyframes(keyframes)
{
}

Matrix Keyframes::CreateMatrix(const Keyframe& keyframe)
{
	return Matrix::CreateScale(keyframe.Scale)
		* Matrix::CreateFromQuaternion(keyframe.Rotation)
		* Matrix::CreateTranslation(keyframe.Translation);
}

Keyframe Keyframes::Interpolate(const Keyframe& lhs, const Keyframe& rhs, float lhsWeight)
{
	Keyframe result;

	result.Translation = Vector3::Lerp(lhs.Translation, rhs.Translation, lhsWeight);
	result.Rotation = Quaternion::Slerp(lhs.Rotation, rhs.Rotation, lhsWeight);
	result.Scale = Vector3::Lerp(lhs.Scale, rhs.Scale, lhsWeight);

	return result;
}

Keyframe Keyframes::Interpolate(float timePos) const
{
	if (mKeyframes.size() == 1)
	{
		return *mKeyframes.begin();
	}

	for (auto iter = mKeyframes.begin() + 1; iter != mKeyframes.end(); ++iter)
	{
		if (iter->TimePos > timePos)
		{
			auto begin = iter - 1;
			float delta = timePos - begin->TimePos;
			float deltaRatio = delta / (iter->TimePos - begin->TimePos);

			return Keyframes::Interpolate(*begin, *iter, deltaRatio);
		}
	}

	return *(mKeyframes.end() - 1);
}

size_t Keyframes::GetKeyframeIndex(float timePos) const
{
	for (auto iter = mKeyframes.begin(); iter != mKeyframes.end(); ++iter)
	{
		if (iter->TimePos > timePos)
		{
			return iter - mKeyframes.begin();
		}
	}

	return -1;
}

AnimClip::AnimClip(aiAnimation* animation)
	: mFrameSize(0u)
{
	double totalFrame = animation->mDuration;
	double framePerSeconds = 1 / (animation->mTicksPerSecond);

	mDuration = totalFrame * framePerSeconds;
	mName = animation->mName.C_Str();

	for (unsigned int j = 0; j < animation->mNumChannels; ++j)
	{
		auto& currentChennel = animation->mChannels[j];

		vector<Keyframe> keyframes;
		keyframes.reserve(currentChennel->mNumPositionKeys);

		for (unsigned int k = 0; k < currentChennel->mNumPositionKeys; ++k)
		{
			Keyframe keyframe;
			keyframe.TimePos = static_cast<float>(currentChennel->mPositionKeys[k].mTime * framePerSeconds);

			keyframe.Translation.x = currentChennel->mPositionKeys[k].mValue.x;
			keyframe.Translation.y = currentChennel->mPositionKeys[k].mValue.y;
			keyframe.Translation.z = currentChennel->mPositionKeys[k].mValue.z;

			keyframe.Rotation.x = currentChennel->mRotationKeys[k].mValue.x;
			keyframe.Rotation.y = currentChennel->mRotationKeys[k].mValue.y;
			keyframe.Rotation.z = currentChennel->mRotationKeys[k].mValue.z;
			keyframe.Rotation.w = currentChennel->mRotationKeys[k].mValue.w;

			keyframe.Scale.x = currentChennel->mScalingKeys[k].mValue.x;
			keyframe.Scale.y = currentChennel->mScalingKeys[k].mValue.y;
			keyframe.Scale.z = currentChennel->mScalingKeys[k].mValue.z;

			keyframes.push_back(keyframe);

			mFrameSize = max(mFrameSize, currentChennel->mNumPositionKeys);
		}

		string name = currentChennel->mNodeName.C_Str();
		mBoneKeyframes.insert({ name, std::make_unique<Keyframes>(name, keyframes) });
	}

	mTimePerFrame = mDuration / mFrameSize;
}

const Keyframes& AnimClip::Getkeyframes(string boneName) const
{
	auto find = mBoneKeyframes.find(boneName);
	assert(find != mBoneKeyframes.end());

	return *(find->second.get());
};

bool AnimClip::TryGetkeyframes(string boneName, const Keyframes** outKeyframes) const
{
	auto find = mBoneKeyframes.find(boneName);

	if (find == mBoneKeyframes.end())
	{
		return false;
	}

	*outKeyframes = find->second.get();

	return true;
}

size_t AnimClip::GetFrameIndex(float timePos, bool bIsRecursive) const
{
	size_t result = static_cast<size_t>(timePos / mTimePerFrame);

	return bIsRecursive ? result % mFrameSize : min(result, mFrameSize - 1);
}

const AnimClip& AnimResource::GetAnimClip(string name) const
{
	auto find = mAnimClips.find(name);
	assert(find != mAnimClips.end());

	return find->second;
}
const AnimClip& AnimResource::GetAnimClip(size_t index) const
{
	assert(index < mAnimClips.size());
	auto find = std::next(mAnimClips.begin(), index);

	return find->second;
}

const set<std::string> AnimResource::GetAnimClipNames() const
{
	std::set<string> names;

	for (auto& element : mAnimClips)
	{
		std::string animName = element.first;
		names.insert(animName);
	}

	return names;
}

AnimResource::AnimResource(const std::string& path)
	: mPath(path)
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	unsigned int importFlags = aiProcess_ConvertToLeftHanded;

	const aiScene* pScene = importer.ReadFile(path, importFlags);

	if (pScene == nullptr)
	{
		assert(false);
	}

	for (unsigned int i = 0; i < pScene->mNumAnimations; ++i)
	{
		aiAnimation* curAnim = pScene->mAnimations[i];

		mAnimClips.emplace(curAnim->mName.C_Str(), curAnim);
	}
}

SpriteAnimation::SpriteAnimation(Vector2 textureSize, Vector2 spriteSize, float duration)
{
	float dTexX = spriteSize.x / textureSize.x;
	float dTexY = spriteSize.y / textureSize.y;

	size_t width = textureSize.x / spriteSize.x;
	size_t height = textureSize.y / spriteSize.y;

	mSpriteClips.reserve(height);

	for (size_t i = 0; i < height; ++i)
	{
		SpriteClip spriteClip;
		spriteClip.Duraition = duration * width;
		spriteClip.Sprites.reserve(width);

		for (size_t j = 0; j < width; ++j)
		{
			Sprite sprite;
			sprite.TexTranslate = { dTexX * j, dTexY * i };
			sprite.TexSize = { dTexX, dTexY };
			sprite.TimePos = duration * j;

			spriteClip.Sprites.push_back(sprite);
		}

		mSpriteClips.push_back(spriteClip);
	}
}

SpriteAnimation::SpriteAnimation(Vector2 textureSize, Vector2 spriteSize, size_t spriteCountY, const std::vector<size_t>& spriteCountX, const std::vector<std::vector<float>>& durations)
{
	float dTexX = spriteSize.x / textureSize.x;
	float dTexY = spriteSize.y / textureSize.y;

	mSpriteClips.reserve(spriteCountY);

	for (size_t i = 0; i < spriteCountY; ++i)
	{
		SpriteClip spriteClip;
		spriteClip.Duraition = 0.f;
		spriteClip.Sprites.reserve(spriteCountX[i]);

		for (size_t j = 0; j < spriteCountX[i]; ++j)
		{
			assert(durations.size() > i);
			assert(durations[i].size() > j);

			spriteClip.Duraition += durations[i][j];

			Sprite sprite;
			sprite.TexTranslate = { dTexX * j, dTexY * i };
			sprite.TexSize = { dTexX, dTexY };
			sprite.TimePos = spriteClip.Duraition;

			spriteClip.Sprites.push_back(sprite);
		}

		mSpriteClips.push_back(spriteClip);
	}
}

const Sprite& SpriteAnimation::GetSprite(size_t x, size_t y, bool bIsRecursive) const
{
	assert(mSpriteClips.size() > y);

	size_t index = bIsRecursive ? x % mSpriteClips[y].Sprites.size() : max(x, mSpriteClips[y].Sprites.size() - 1);

	return mSpriteClips[y].Sprites[index];
}
const Sprite& SpriteAnimation::GetSprite(size_t y, float timePos, bool bIsRecursive) const
{
	assert(mSpriteClips.size() > y);

	timePos = bIsRecursive ? fmod(timePos, mSpriteClips[y].Duraition) : fmax(timePos, mSpriteClips[y].Duraition);

	for (auto iter = mSpriteClips[y].Sprites.begin(); iter != mSpriteClips[y].Sprites.end(); ++iter)
	{
		if (iter->TimePos > timePos)
		{
			auto find = std::next(iter, -1);

			return *find;
		}
	}

	return *mSpriteClips[y].Sprites.begin();
}
