#ifndef __ELGUIImagesetManager_H__
#define __ELGUIImagesetManager_H__

#include "ElGUISingleton.h"
#include "ELGUITexture.h"
#include "ELGUISize.h"
#include <string>
#include <map>

namespace ELGUI
{
	class Imageset;

	class ImagesetManager : public Singleton<ImagesetManager>
	{
	public:
		ImagesetManager();
		virtual ~ImagesetManager();

		static ImagesetManager& getSingleton();
		static ImagesetManager* getSingletonPtr();

		Imageset* createImageset(const std::string& name, Texture* texture);
		
		void destroyImageset(const std::string& name);
		void destroyImageset(Imageset* imageset);
		void destroyAllImagesets();

		Imageset* getImageset(const std::string& name) const;
		bool isImagesetPresent(const std::string& name) const
		{
			return d_imagesets.find(name.c_str()) != d_imagesets.end();
		}

		void notifyScreenResolution(const Size& size);

	protected:
		typedef	std::map<std::string, Imageset*> ImagesetRegistry;
		ImagesetRegistry	d_imagesets;
	};
}

#endif //__ELGUIImagesetManager_H__