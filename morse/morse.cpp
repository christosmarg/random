#include <iostream>
#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

static void
play(Mix_Chunk *sound)
{
	Mix_PlayChannel(-1, sound, 0);	  
	while (Mix_Playing(-1) != 0)
		SDL_Delay(200);
}

int
main(int argc, char *argv[])
{
	Mix_Chunk *dot	= nullptr;
	Mix_Chunk *dash = nullptr;
	std::string in, out;
	std::map<char, std::string>::const_iterator it;
	std::map<char, std::string> morsetable = {
		{'a', ".-"     }, {'A', ".-"	 }, 
		{'b', "-..."   }, {'B', "-..."	 },
		{'c', "-.-."   }, {'C', "-.-."	 },
		{'d', "-.."    }, {'D', "-.."	 },
		{'e', "."      }, {'E', "."	 },   
		{'f', "..-."   }, {'F', "..-."	 },
		{'g', "--."    }, {'G', "--."	 },
		{'h', "...."   }, {'H', "...."	 },
		{'i', ".."     }, {'I', ".."	 }, 
		{'j', ".---"   }, {'J', ".---"	 },
		{'k', "-.-"    }, {'K', "-.-"	 },
		{'l', ".-.."   }, {'L', ".-.."	 },
		{'m', "--"     }, {'M', "--"	 }, 
		{'n', "-."     }, {'N', "-."	 }, 
		{'o', "---"    }, {'O', "---"	 },
		{'p', ".--."   }, {'P', ".--."	 },
		{'q', "--.-"   }, {'Q', "--.-"	 },
		{'r', ".-."    }, {'R', ".-."	 },
		{'s', "..."    }, {'S', "..."	 },
		{'t', "-"      }, {'T', "-"	 },   
		{'u', "..-"    }, {'U', "..-"	 },
		{'v', "...-"   }, {'V', "...-"	 },
		{'w', ".--"    }, {'W', ".--"	 },
		{'x', "-..-"   }, {'X', "-..-"	 },
		{'y', "-.--"   }, {'Y', "-.--"	 },
		{'z', "--.."   }, {'Z', "--.."	 },
		{'&', ".-..."  }, {'\'', ".----."},
		{'@', ".--.-." }, {':', "---..." },  
		{'(', "-.--."  }, {')', "-.--.-" },
		{',', "--..--" }, {'=', "-...-"  },
		{'!', "-.-.--" }, {'.', ".-.-.-" },
		{'-', "-....-" }, {'+', ".-.-."  },
		{'\"', ".-..-."}, {'\?', "..--.."},
		{'/', "-..-."  }, {' ', " / "	 }
	};

	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		return (1);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096);
	dot  = Mix_LoadWAV("dot.wav");
	dash = Mix_LoadWAV("dash.wav");

	std::cout << "Press CTRL+C to exit" << std::endl;
	for (;;) {
		std::cout << "> ";
		std::getline(std::cin, in);
		for (const char& c : in) {
			it = morsetable.find(c);
			if (morsetable.find(c) != morsetable.end())
				out += it->second;
			else
				std::cerr << "Character '" << c <<
				    "' not found." << std::endl;
			out += " ";
		}
		if (!out.empty()) {
			std::cout << out << std::endl;
			for (const char& c : out) {
				if (c == '.')
					play(dot);
				else if (c == '-')
					play(dash);
			}
			out.clear();
		}
	}

	Mix_FreeChunk(dot);
	Mix_FreeChunk(dash);
	Mix_CloseAudio();
	SDL_Quit();

	return (0);
}
