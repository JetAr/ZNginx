#include "ELGUIPCH.h"
#include "ELGUIImagesetManager.h"
#include "ELGUIImageset.h"

namespace ELGUI
{
	template<> ImagesetManager* Singleton<ImagesetManager>::ms_Singleton = 0;

	ImagesetManager::ImagesetManager()
	{

	}

	ImagesetManager::~ImagesetManager()
	{
		destroyAllImagesets();
	}

	ImagesetManager& ImagesetManager::getSingleton()
	{
		return Singleton<ImagesetManager>::getSingleton();
	}

	ImagesetManager* ImagesetManager::getSingletonPtr()
	{
		return Singleton<ImagesetManager>::getSingletonPtr();
	}

	Imageset* ImagesetManager::createImageset(const std::string& name, Texture* texture)
	{
		if (isImagesetPresent(name))
		{
			assert(0 && std::string("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.").c_str());
		}

		Imageset* temp = new Imageset(name, texture);
		d_imagesets[name.c_str()] = temp;

		return temp;
	}

	void ImagesetManager::destroyImageset(const std::string& name)
	{
		ImagesetRegistry::iterator	pos = d_imagesets.find(name.c_str());

		if (pos != d_imagesets.end())
		{
			delete pos->second;
			d_imagesets.erase(pos);
		}

	}

	void ImagesetManager::destroyImageset(Imageset* imageset)
	{
		if (imageset)
		{
			destroyImageset(imageset->getName());
		}
	}

	void ImagesetManager::destroyAllImagesets()
	{
		while (!d_imagesets.empty())
		{
			destroyImageset(d_imagesets.begin()->first);
		}
	}

	Imageset* ImagesetManager::getImageset(const std::string& name) const
	{
		ImagesetRegistry::const_iterator	pos = d_imagesets.find(name.c_str());

		if (pos == d_imagesets.end())
		{
			assert(0 && std::string("ImagesetManager::getImageset - No Imageset named '" + name + "' is present in the system.").c_str());
		}

		return pos->second;
	}


	void ImagesetManager::notifyScreenResolution(const Size& size)
	{
		// notify all attached Imageset objects of the change in resolution
		ImagesetRegistry::iterator pos = d_imagesets.begin(), end = d_imagesets.end();

		for (; pos != end; ++pos)
		{
			pos->second->notifyScreenResolution(size);
		}
	}
}