#pragma once

#include "Source.h"
#include "IBindable.h"
#include "BufferResource.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

class IBindable;

namespace Rgph
{
	class Pass;

	class Sink
	{
	public:
		Sink() = default;
		virtual ~Sink() = default;

		virtual void BindSink(Source& source) = 0;
		virtual void PostLinkValidate() const = 0;

		void SetTarget(const std::string& passName, const std::string& outputName);

		const std::string& GetRegisteredName() const;
		const std::string& GetPassName() const;
		const std::string& GetOutputName() const;

	protected:
		Sink(std::string registeredName);

	private:
		std::string mRegisteredName;
		std::string mPassName;
		std::string mOutputName;
	};

	template<class T>
	class DirectBufferSink : public Sink
	{
		static_assert(std::is_base_of_v<Bind::BufferResource, T>, "T mush be BufferResource");

	public:
		DirectBufferSink(std::string registeredName, std::shared_ptr<T>& bind);
		virtual ~DirectBufferSink() = default;

		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target);

		void PostLinkValidate() const override;
		void BindSink(Source& source) override;

	private:
		std::shared_ptr<T>& mTarget;
		bool mbIsLinked = false;
	};

#pragma region DirectBufferSinkFunc
	template<class T>
	DirectBufferSink<T>::DirectBufferSink(std::string registeredName, std::shared_ptr<T>& bind)
		: Sink(std::move(registeredName))
		, mTarget(bind)
	{
	}

	template<class T>
	std::unique_ptr<Sink> DirectBufferSink<T>::Make(std::string registeredName, std::shared_ptr<T>& target)
	{
		return std::make_unique<DirectBufferSink>(std::move(registeredName), target);
	}

	template<class T>
	void DirectBufferSink<T>::PostLinkValidate() const
	{
		assert(mbIsLinked);
	}

	template<class T>
	void DirectBufferSink<T>::BindSink(Source& source)
	{
		auto p = std::dynamic_pointer_cast<T>(source.YieldBuffer());
		assert(p != nullptr);

		mTarget = std::move(p);
		mbIsLinked = true;
	}
#pragma endregion

	template<class T>
	class ContainerBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<IBindable, T>, "T mush be IBindable");

	public:
		ContainerBindableSink(std::string registeredName, std::vector<std::shared_ptr<IBindable>>& container, size_t index);
		virtual ~ContainerBindableSink() = default;

		void PostLinkValidate() const override;
		void BindSink(Source& source) override;

	private:
		std::vector<std::shared_ptr<IBindable>>& mContainer;
		size_t mIndex;
		bool mbIsLinked = false;
	};

#pragma region ContainerBindableSinkFunc
	template<class T>
	ContainerBindableSink<T>::ContainerBindableSink(std::string registeredName, std::vector<std::shared_ptr<IBindable>>& container, size_t index)
		: Sink(std::move(registeredName))
		, mContainer(container)
		, mIndex(index)
	{}

	template<class T>
	void ContainerBindableSink<T>::PostLinkValidate() const
	{
		assert(mbIsLinked);
	}
	template<class T>
	void ContainerBindableSink<T>::BindSink(Source& source)
	{
		auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
		assert(p != nullptr);

		mContainer[mIndex] = std::move(p);
		mbIsLinked = true;
	}
#pragma endregion

	template<class T>
	class DirectBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<IBindable, T>, "T mush be IBindable");

	public:
		DirectBindableSink(std::string registeredName, std::shared_ptr<T>& target);
		virtual ~DirectBindableSink() = default;

		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target);

		void PostLinkValidate() const override;
		void BindSink(Source& source) override;

	private:
		std::shared_ptr<T>& mTarget;
		bool mbIsLinked = false;
	};

#pragma region DirectBindableSinkFunc
	template<class T>
	DirectBindableSink<T>::DirectBindableSink(std::string registeredName, std::shared_ptr<T>& target)
		: Sink(std::move(registeredName))
		, mTarget(target)
	{
	}

	template<class T>
	std::unique_ptr<Sink> DirectBindableSink<T>::Make(std::string registeredName, std::shared_ptr<T>& target)
	{
		return std::make_unique<DirectBindableSink>(std::move(registeredName), target);
	}

	template<class T>
	void DirectBindableSink<T>::PostLinkValidate() const
	{
		assert(mbIsLinked);
	}

	template<class T>
	void DirectBindableSink<T>::BindSink(Source& source)
	{
		auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
		assert(p != nullptr);
		mTarget = std::move(p);
		mbIsLinked = true;
	}
#pragma endregion
}