#pragma once

class IBindable;

namespace Bind
{
	class BufferResource;
}

namespace Rgph
{
	class Source
	{
	public:
		virtual ~Source() = default;

		virtual void PostLinkValidate() const = 0;

		virtual std::shared_ptr<IBindable> YieldBindable();
		virtual std::shared_ptr<Bind::BufferResource> YieldBuffer();

		const std::string& GetName() const;

	protected:
		Source(std::string name);

	private:
		std::string mName;
	};

	template<class T>
	class DirectBufferSource : public Source
	{
	public:
		DirectBufferSource(std::string name, std::shared_ptr<T>& buffer);
		virtual ~DirectBufferSource() = default;

		static std::unique_ptr<DirectBufferSource<T>> Make(std::string name, std::shared_ptr<T>& buffer);

		void PostLinkValidate() const;
		std::shared_ptr<Bind::BufferResource> YieldBuffer() override;

	private:
		std::shared_ptr<T>& mBuffer;
		bool mbIslinked = false;
	};

#pragma region DirectBufferSourceFunc
	template<class T>
	DirectBufferSource<T>::DirectBufferSource(std::string name, std::shared_ptr<T>& buffer)
		: Source(std::move(name))
		, mBuffer(buffer)
	{
	}

	template<class T>
	std::unique_ptr<DirectBufferSource<T>> DirectBufferSource<T>::Make(std::string name, std::shared_ptr<T>& buffer)
	{
		return std::make_unique<DirectBufferSource>(std::move(name), buffer);
	}

	template<class T>
	void DirectBufferSource<T>::PostLinkValidate() const
	{
	}

	template<class T>
	std::shared_ptr<Bind::BufferResource> DirectBufferSource<T>::YieldBuffer()
	{
		assert(!mbIslinked);
		mbIslinked = true;
		return mBuffer;
	}

#pragma endregion

	template<class T>
	class DirectBindableSource : public Source
	{
	public:
		DirectBindableSource(std::string name, std::shared_ptr<T>& bind);
		virtual ~DirectBindableSource() = default;

		static std::unique_ptr<DirectBindableSource> Make(std::string name, std::shared_ptr<T>& buffer);

		void PostLinkValidate() const;
		std::shared_ptr<IBindable> YieldBindable() override;

	private:
		std::shared_ptr<T>& mBind;
	};

#pragma region DirectBindableSourceFunc
	template<class T>
	DirectBindableSource<T>::DirectBindableSource(std::string name, std::shared_ptr<T>& bind)
		: Source(std::move(name))
		, mBind(bind)
	{
	}

	template<class T>
	std::unique_ptr<DirectBindableSource<T>> DirectBindableSource<T>::Make(std::string name, std::shared_ptr<T>& buffer)
	{
		return std::make_unique<DirectBindableSource>(std::move(name), buffer);
	}

	template<class T>
	void DirectBindableSource<T>::PostLinkValidate() const
	{
	}

	template<class T>
	std::shared_ptr<IBindable> DirectBindableSource<T>::YieldBindable()
	{
		return mBind;
	}
#pragma endregion
}