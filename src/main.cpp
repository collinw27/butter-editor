
#include <SFML/Graphics.hpp>

#include "editor/Editor.h"
#include "utility/FileManager.h"
#include "utility/Graphics.h"
#include "utility/Logger.h"
#include "utility/Input.h"
#include "utility/Debugger.h"
#include "utility/core.h"

int main()
{
    try
    {
        FileManagerSingleton* file_manager = new FileManagerSingleton();
        GraphicsSingleton* graphics = new GraphicsSingleton();
        LoggerSingleton* logger = new LoggerSingleton();
        InputSingleton* input_manager = new InputSingleton();
        DebuggerSingleton* debugger = new DebuggerSingleton();
        Editor* editor = new Editor();

        editor->run();
        
        delete editor;
        delete file_manager;
        delete graphics;
        delete logger;
        delete input_manager;
        delete debugger;
    }
    catch(const ButterException& e)
    {
        e.print();
    }

    return 0;
}