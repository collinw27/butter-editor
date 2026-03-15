
#include <SFML/Graphics.hpp>

#include "editor/Editor.h"
#include "utility/FileManager.h"
#include "utility/Logger.h"
#include "utility/Input.h"
#include "utility/Exceptions.h"

int main()
{
    try
    {
        FileManagerSingleton* resource_manager = new FileManagerSingleton();
        LoggerSingleton* logger = new LoggerSingleton();
        InputSingleton* input_manager = new InputSingleton();
        Editor* editor = new Editor();

        editor->run();
        
        delete editor;
        delete resource_manager;
        delete logger;
        delete input_manager;
    }
    catch(const ButterException& e)
    {
        e.print();
    }

    return 0;
}