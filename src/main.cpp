
#include <SFML/Graphics.hpp>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#include "editor/Editor.h"
#include "utility/ResourceManager.h"
#include "utility/Logger.h"
#include "utility/Input.h"
#include "utility/Exceptions.h"

int main()
{
    try
    {
        ResourceManagerSingleton* resource_manager = new ResourceManagerSingleton();
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