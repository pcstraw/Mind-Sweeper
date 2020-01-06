#pragma once
#include "NativeHeader.h"

nativenamespace{

	class GLResource
	{
	protected:
		bool _deleted = false;
		int _index = -1;
		
	public:

		unsigned int id;

		void AddToResources()
		{
			AddResource(this);
		}

		static std::unordered_map<unsigned int,GLResource*> Resources;

		static void AddResource(GLResource* resource)
		{
			Resources.insert(std::make_pair(resource->id,resource));
		}

		static void DeleteResource(unsigned int ID)
		{

			if (&Resources[ID] != NULL)
			{
				Resources[ID]->Delete();
				Resources.erase(ID);
			}
		}

		static void DeleteResource(GLResource& resource)
		{
			resource.Delete();
			Resources.erase(resource.id);
		}

		static void ClearResources()
		{

			for (std::pair<unsigned int, GLResource*> r : Resources)
			{
				r.second->Delete();
				delete r.second;
				r.second = NULL;
			}
			Resources.clear();
			/*
			std::unordered_map<unsigned int,GLResource>::iterator it = Resources.begin();
			while (it != Resources.end())
			{
				it->second.Delete();
				it++;
			}
			*/
		}


		virtual void Delete() {}
	};
}
