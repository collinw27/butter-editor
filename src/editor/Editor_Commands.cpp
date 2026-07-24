#include "editor/Editor.h"

#include "command/exceptions.h"

enum {
    CMD_LOG,
    CMD_UI_SCALE,
    CMD_TYPE_TEST,
    CMD_NEW,
    CMD_SAVE,
    CMD_SAVE_AS,
    CMD_LOAD,
    CMD_CREATE_CLIP
};

void Editor::initialize_commands()
{
    command_parser = CommandParser();
    command_parser.define_command(command_parser.new_command("log", (int) CMD_LOG)
        .add_parameter("value", CommandParser::ParamType::STRING)
    );
    command_parser.define_command(command_parser.new_command("ui_scale", (int) CMD_UI_SCALE)
        .add_parameter("value", CommandParser::ParamType::INT)
    );
    command_parser.define_command(command_parser.new_command("type_test", (int) CMD_TYPE_TEST)
        .add_parameter("int_value", CommandParser::ParamType::INT)
        .add_parameter("uint_value", CommandParser::ParamType::U_INT)
        .add_parameter("string_value", CommandParser::ParamType::STRING)
        .add_parameter("float_value", CommandParser::ParamType::FLOAT)
        .add_parameter("bool_value", CommandParser::ParamType::BOOL)
    );
    command_parser.define_command(command_parser.new_command("new", (int) CMD_NEW));
    command_parser.define_command(command_parser.new_command("save", (int) CMD_SAVE));
    command_parser.define_command(command_parser.new_command("save_as", (int) CMD_SAVE_AS)
        .add_parameter("name", CommandParser::ParamType::STRING)
    );
    command_parser.define_command(command_parser.new_command("load", (int) CMD_LOAD)
        .add_parameter("name", CommandParser::ParamType::STRING)
    );
    command_parser.define_command(command_parser.new_command("create_clip", (int) CMD_CREATE_CLIP)
        .add_parameter("start", CommandParser::ParamType::U_INT)
        .add_parameter("length", CommandParser::ParamType::U_INT)
        .add_parameter("color", CommandParser::ParamType::STRING)
    );
}

std::string Editor::execute_command(CommandResult command)
{
    // Command ID allows switch to be used, which is better practice
    // than a long chain of if-else with strings

    int root = command.get_root_id();

    switch (root)
    {
    case CMD_LOG:
    
        return "* " + command.get_string(0);

    case CMD_UI_SCALE:
    
        command_parser.validate_range(command.get_int(0), -3, 10);
        ui_scale_index = command.get_int(0);
        ui_scale = 1.f + (float)ui_scale_index * 0.1f;
        resize_modules();
        return "Set UI scale to " + std::to_string(ui_scale_index) + ".";

    case CMD_TYPE_TEST:
    
        command_parser.validate_range(command.get_float(3), 0.0, 99999.0);
        break;

    case CMD_NEW:

        if (project)
            delete project;
        project = new Project();
        command_bar->set_status_text(project->get_name());
        timeline_module->refresh_clips();
        return "Created new project.";

    case CMD_SAVE:

        if (!project->named())
            throw ExecuteException("Unnamed projects must use `save_as`.");
        project->save();
        return "Saved project \"" + project->get_name() + "\".";

    case CMD_SAVE_AS:
    
        project->set_name(command.get_string(0));
        command_bar->set_status_text(project->get_name());
        project->save();
        return "Saved project \"" + project->get_name() + "\".";

    case CMD_LOAD:
    
        if (!Project::exists(command.get_string(0)))
            throw ExecuteException("Nonexistent project \"" + command.get_string(0) + "\".");
        if (project)
            delete project;
        project = new Project(command.get_string(0));
        command_bar->set_status_text(project->get_name());
        timeline_module->refresh_clips();
        return "Loaded project \"" + project->get_name() + "\".";
        
    case CMD_CREATE_CLIP:

        int c_index = -1;
        std::string provided_name = command.get_string(2);
        std::vector<std::string> c_names = {"red", "orange", "yellow", "green", "blue", "purple"};
        for (int i = 0; i < c_names.size(); ++i)
        {
            if (provided_name == c_names.at(i) || (provided_name.length() == 1 && provided_name.at(0) == c_names.at(i).at(0)))
                c_index = i;
        }
        if (c_index == -1)
            throw ExecuteException("Invalid color");
        std::vector<std::string> colors = {"#ff5959", "#ffa75e", "#ffec5e", "#63ff73", "#73beff", "#e678ff"};
        if (command.get_int(1) <= 0)
            throw ExecuteException("Clip length must be positive.");
        bool successful = project->add_color_clip(command.get_int(0), command.get_int(1), hex_to_color(colors.at(c_index)));
        timeline_module->refresh_clips();
        return successful ? "Created clip." : "Could not create clip.";
    
    }
        
    return "Execution was successful.";
}