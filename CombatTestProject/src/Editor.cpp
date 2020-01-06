#include "Editor.h"
#include "AssetDatabase.h"
#include "Serializer.h"

using nativenamespace;

Editor Native::Editor::instance;
bool Native::EditorGUI::_gimbalDragging = false;
Assets Native::Assets::instance;
std::ofstream Native::Save::_outstream;
std::ifstream Native::Save::_instream;
