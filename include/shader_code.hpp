#include <string>

class ShaderCode
{
public:
    ShaderCode()
    {
        play_vertex_shader = 
            "#version 120\n"
            "attribute vec2 aPos;\n"
            "attribute vec2 aTexCoord;\n"
            "varying vec2 TexCoord;\n"
            "void main() {\n"
                "gl_Position = vec4(aPos, 1.0, 1.0);\n"
                "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
            "}"
        ;

        play_frag_shader = 
            "#version 120\n"
            "varying vec2 TexCoord;\n"
            "uniform sampler2D texture;\n"
            "uniform int uPlaySelected;\n"
            "void main() {\n"
                "vec4 color = texture2D(texture, TexCoord);\n"
                "if(uPlaySelected == 0 || TexCoord.x < 0.95 && TexCoord.x > 0.04 && TexCoord.y < 0.95 && TexCoord.y > 0.04){\n"
                    "gl_FragColor = color;\n"
                "}else {\n" //draw border
                    "gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n" 
                "}\n"
            "}"
        ;

    }

    const char* get_play_vertex()
    {
        return play_vertex_shader.c_str();;
    }

    const char* get_play_frag()
    {
        return play_frag_shader.c_str();;
    }

private:
    std::string play_vertex_shader;
    std::string play_frag_shader;
    std::string quit_vertex_shader;
    std::string quit_frag_shader;
};

