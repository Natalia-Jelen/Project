#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include <iostream>
#include <vector>
#include<fstream>
#include<string>
#include<sstream>

using namespace std;

//dane kamery
//wektor pozycji kamery
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 30.0f);
//wektor wskazuj¹cy kierunek patrzenia kamery
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//wektor pomocniczy: iloczyn wektorowy cameraUp i cameraFront daje nam wektor odpowiedzialny za ruch kamery w jej osi x
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//k¹t odpowiedzialny za pole widzenia kamery
float fov = 45.0f;
//k¹t odpowiedzialny za rozgl¹danie siê kamer¹ lewa/prawa
float yaw = -90.0f;
//k¹t odpowiedzialny za rozgl¹danie siê kamer¹ góra/dó³
float pitch = 0.0f;

bool firstMouse = true;
float lastX;//pozycja x kursora przed jego przemieszczeniem
float lastY;//pozycja y kursora przed jego przemieszczeniem


//pozycja œwiat³a
glm::vec3 lightPos(5.0f, 5.0f, 4.0f);



void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{// xpos, ypos pozycja kursora na ekranie 2D liczona wzglêdem lewego górnego naro¿nika client area okna programu
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//Obliczanie o ile przesun¹³ siê kursor wzglêdem swojej poprzedniej pozycji
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	
	//zapis aktualnej pozycji kursora jako pozycji ostatniej, ¿eby w kolejnym wywo³aniu tej funkcji (co siê równa przemieszczeniu kursora) mo¿na by³o obliczyæ o ile przesun¹³
	//siê kursor
	lastX = xpos;
	lastY = ypos;
	//sensitivity to "czu³oœæ" ruchu kursora
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//zabezpieczenie ruchu kamer¹ (ruch góra/dó³)
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	//obliczanie nowego kierunku patrzenia kamery
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}


void processInput(GLFWwindow *window)
{
	const float cameraSpeed = 0.2f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//light movement
	const float lightSpeed = 0.1f;
	//x+
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		lightPos.x += lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	//x-
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		lightPos.x -= lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	//y+
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		lightPos.y += lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	//y-
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		lightPos.y -= lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	//z+
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		lightPos.z += lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	//z-
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		lightPos.z -= lightSpeed;
		std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
	}
	
}
//funkcja wczytuje kod shaderów z plików tekstowych do tablicy typu char i zwraca adres na 
//pierwszy element tej tablicy
char *Load_Shader(const char *fname)
{

	FILE *file = fopen(fname, "rb");
	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = (char *)malloc(sizeof(char) * (fsize + 1));
	fread(buffer, 1, fsize, file);
	buffer[fsize] = 0;

	fclose(file);

	return buffer;
}
//funkcja ta tworzy obiekt shader, umieszcza w nim kod z pliku tekstowego, kompiluje kod, sprawdza 
//status kompilacji, wyœwietla informacjê w konsoli jeœli kompilacja jest nieudana
int Create_Vertex_Shader(const char *fname)
{
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char *vertexShaderSource = Load_Shader(fname);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(vertexShaderSource);
	return vertexShader;
}

int Create_Fragment_Shader(const char *fname)
{
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char *vertexShaderSource = Load_Shader(fname);
	glShaderSource(fragmentShader, 1, &vertexShaderSource, NULL);
	glCompileShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(vertexShaderSource);
	return fragmentShader;
}

int Create_Program(int vertexShader, int fragmentShader)
{
	int success;
	char infoLog[512];

	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	return shaderProgram;
}

class Object
{
public:
	
	int liczba_bajtow_do_renderu;//zawartoœæ tablicy wskazywanej przez wskaŸnik wskaznik_vert w bajtach
	int liczba_bajtow_indexow;//zawartoœæ tablicy wskazywanej przez wskaŸnik wskaznik_inde w bajtach
	int liczba_inde;//liczba indexów, których u¿yjemy do rysowania obiektów w funkcji draw_mesh, indexy te bêd¹ zapisane w tablicy wskazywanej przez wskaŸnik wskaznik_inde

	//wskaŸnik na tablicê vertexów niepowtarzalnych (utworzon¹ w funkcji load_objn_mesh)
	float *wskaznik_vert;
	//wskaŸnik na tablicê indexów (utworzon¹ w funkcji load_objn_mesh)
	int *wskaznik_inde;
	
	unsigned int textures[2];
	unsigned int *tex[2];


	Object(int wartosc)
	{
		
		liczba_inde = wartosc;
		
		
	}

	void load_objn_mesh(string filename)
	{

		string wiersz;//wiersz danych w formie string
		string x, y, z;//wspó³rzêdne vertexa w formie string
		string u, v;// wspó³rzêdne uv w formie string
		string xn, yn, zn;//wspó³rzêdne wektora normalnego w formie string
		string wiersz_poz_1vert, wiersz_poz_2vert, wiersz_poz_3vert;// numery wierszy w .obj dla pozycji vertexów danego trójk¹ta
		string wiersz_uv_1vert, wiersz_uv_2vert, wiersz_uv_3vert;//numery wierszy w .obj dla wspó³rzêdnych uv vertexów danego trójk¹ta
		string wiersz_normalnego_1vert, wiersz_normalnego_2vert, wiersz_normalnego_3vert;//numery wierszy w .obj dla wektorów normalnych vertexów danego trójk¹ta
		string niepotrzebny;//niepotrzebny fragment wiersza z .obj
		ifstream file;
		int numer_pierwszego_wiersza_poz_vert = 3;
		int liczba_wierszy_pos = 0;//liczba vertexów niepowtarzalnych
		int liczba_wierszy_uv = 0;//liczba uv niepowtarzalnych
		int liczba_wierszy_normal = 0;//liczba normalnych niepowtarzalnych
		
		int pos_index;
		int uv_index;
		int nor_index;
		float xfloat, yfloat, zfloat, ufloat, vfloat, xnfloat, ynfloat, znfloat;//tymczasowe floaty
		
		bool vertexy = true;
		bool uv_ki = true;
		bool normalne = true;
		bool trojkaty = true;


		file.open(filename);
		if (file.good() == false)
		{
			cout << "Nie mozna otworzyc pliku!";
		}

		for (int index = 0; index < numer_pierwszego_wiersza_poz_vert; index++)//pierwsze 3 wiersze w .obj nas nie interesuj¹ dlatego przechodzimy przez nie do pierwszego vertexa w .obj
		{
			getline(file, wiersz);
		}

		//obliczamy ile jest wierszy z pozycjami vertexów
		while (vertexy)
		{
			getline(file, wiersz);//pobieramy wiersz danych
			if (wiersz.find("vt") != string::npos)//jeœli znajdziemy "vt" to if siê wykona. Funkcja Find() zwraca pozycjê szukanego wyra¿enia "vt" w "wiersz", jeœli 
			{									  //nie znajdzie szukanego wyra¿enia to zwraca "string::npos"
				vertexy = false;
				liczba_wierszy_uv++;//zaczynamy liczyæ wiersze z wspó³rzêdnymi "uv"
			}
			else
			{
				liczba_wierszy_pos++;//liczba wierszy z pozycjami vertexów
			}

		}
		//kontynuujemy liczenie wierszy ze wspó³rzêdnymi "uv"
		while (uv_ki)
		{
			getline(file, wiersz);
			if (wiersz.find("vn") != string::npos)//wykonujemy to samo co wczeœniej z t¹ ró¿nic¹, ¿e tutaj szukamy "vn"
			{
				uv_ki = false;
				liczba_wierszy_normal++;//zaczynamy liczyæ wiersze z wektorami normalnymi
			}
			else
			{
				liczba_wierszy_uv++;//liczba wierszy z "uv"
			}

		}
		//kontynuujemy liczenie wierszy z wektorami normalnymi
		while (normalne)//obliczanie poz pocz uv i pamieci dla v_list
		{
			getline(file, wiersz);
			if (wiersz.find("s off") != string::npos)
			{
				normalne = false;
			}
			else
			{
				liczba_wierszy_normal++;//kontynuacja liczenia wierszy z normalnymi
			}

		}

		while (trojkaty)//obliczanie potrzebnych pamieci do zarezerwowania
		{
			getline(file, wiersz);
			if (wiersz.find("f") != string::npos)
			{
				liczba_inde += 3;// +3 bo s¹ 3 indexy vertexów na trójk¹t
			}
			else
			{
				trojkaty = false;
			}

		}
		
		wskaznik_inde = new int[liczba_inde];
		int *ptr_numery_wierszy_poz = new int[liczba_inde];//liczba indexów w pliku obj dla vertexów jest taka sama jak liczba indexow dla uv i normalnych
		int *ptr_numery_wierszy_uv = new int[liczba_inde];
		int *ptr_numery_wierszy_norm = new int[liczba_inde];
		
		string *ptr_v_string = new string[liczba_wierszy_pos];
		string *ptr_uv_string = new string[liczba_wierszy_uv];
		string *ptr_vn_string = new string[liczba_wierszy_normal];

		file.clear();
		file.seekg(0);//przechodzimy na pocz¹tek danych pliku .obj

		for (int index = 0; index < numer_pierwszego_wiersza_poz_vert; index++)//pierwsze 3 wiersze w .obj nas nie interesuj¹ dlatego przechodzimy przez nie do pierwszego vertexa w .obj
		{
			getline(file, wiersz);
		}

		//pobieranie z .obj wierszy z danymi pozycji  w formie string i umieszczanie ich w tablicy typu string ptr_v_string
		for (int index = 0; index < liczba_wierszy_pos; index++)//bêd¹c na pozycji pierwszego wiersza pozycji vertexa w.obj pobieram wiersz po wierszu 
		{
			getline(file, ptr_v_string[index]);
		}

		//pobieranie z .obj wierszy z danymi uv  w formie string i umieszczanie ich w tablicy typu string ptr_uv_string
		for (int index = 0; index < liczba_wierszy_uv; index++)// to samo co z wierszami pozyccji vertexów robimy z wierszami uv
		{
			getline(file, ptr_uv_string[index]);
		}

		//pobieranie z .obj wierszy z danymi normalnymi  w formie string i umieszczanie ich w tablicy typu string ptr_vn_string
		for (int index = 0; index < liczba_wierszy_normal; index++)
		{
			getline(file, ptr_vn_string[index]);
		}

		getline(file, wiersz);
		for (int index = 0; index < liczba_inde; index += 3)
		{
			//pobranie z .obj wiersza z numerami wierszy dla pozycji, uv-ki, wektora normalnego trzech vertexów danego trójk¹ta
			getline(file, wiersz);
			stringstream trojkat_string(wiersz);//pobieranie z pobranego wiersza samych numerów wierszy dla pozycji, uv-ki, wektora normalnego trzech vertexów danego trójk¹ta
			getline(trojkat_string, niepotrzebny, ' ');
			getline(trojkat_string, wiersz_poz_1vert, '/');
			getline(trojkat_string, wiersz_uv_1vert, '/');
			getline(trojkat_string, wiersz_normalnego_1vert, ' ');
			getline(trojkat_string, wiersz_poz_2vert, '/');
			getline(trojkat_string, wiersz_uv_2vert, '/');
			getline(trojkat_string, wiersz_normalnego_2vert, ' ');
			getline(trojkat_string, wiersz_poz_3vert, '/');
			getline(trojkat_string, wiersz_uv_3vert, '/');
			getline(trojkat_string, wiersz_normalnego_3vert, '\n');

			//1 vertex trójk¹ta
			ptr_numery_wierszy_poz[index] = stoi(wiersz_poz_1vert) - 1;//odejmujemy 1, poniewa¿ pozycje wierszy w .obj liczone s¹ od 1, a my bêdziemy u¿ywaæ tych pozycji jako indexów w tablicach
			ptr_numery_wierszy_uv[index] = stoi(wiersz_uv_1vert) - 1;// funkcja stoi konwertuje string na int
			ptr_numery_wierszy_norm[index] = stoi(wiersz_normalnego_1vert) - 1;

			//2 vertex trójk¹ta
			ptr_numery_wierszy_poz[index + 1] = stoi(wiersz_poz_2vert) - 1;
			ptr_numery_wierszy_uv[index + 1] = stoi(wiersz_uv_2vert) - 1;
			ptr_numery_wierszy_norm[index + 1] = stoi(wiersz_normalnego_2vert) - 1;

			//3 vertex trójk¹ta
			ptr_numery_wierszy_poz[index + 2] = stoi(wiersz_poz_3vert) - 1;
			ptr_numery_wierszy_uv[index + 2] = stoi(wiersz_uv_3vert) - 1;
			ptr_numery_wierszy_norm[index + 2] = stoi(wiersz_normalnego_3vert) - 1;
		}

		
		std::vector<float>dane_vertexow;
		int zajete_miejsca = 0;//liczba skompletowanych vertexów (jeden skompletowany vertex: x,y,z,u,v,xn,yn,zn) w vectorze dane_vertexow
		bool juz_jest;// jeœli skompletowane vertexy siê powtarzaj¹
		int index_kolejnego_indexa = 0;//czyli miejsce w tablicy indexów do zapisania indexa skompletowanego vertexa
		int index_kolejnego_vertexa = 0;//czyli index skompletowanego vertexa
		//w tej pêtli u¿ywamy indexów z pliku obj ¿eby po³aczyæ dane pozycji vertexa z danymi uv i danymi normalnego dla tego vertexa (razem mamy 8 floatów x,y,z,u,v,xn,yn,zn)
		//konwertujemy dane vertexów ze typu string na float, sprawdzamy czy dane skompletowanych vertexów siê powtarzaj¹, zapisujemy te skompletowane vertexy oraz ich indexy
		for (int index = 0; index < liczba_inde; index++)//dla kazdego vertexa
		{
			juz_jest = false;
			pos_index = ptr_numery_wierszy_poz[index];
			uv_index = ptr_numery_wierszy_uv[index];
			nor_index = ptr_numery_wierszy_norm[index];
			//kompletujemy dane vertexa jeszcze w formie string
			stringstream v_string(ptr_v_string[pos_index]);//tutaj widaæ dlaczego odejmowaliœmy 1 od wszystkich pozycji danych 
			v_string >> niepotrzebny >> x >> y >> z;// ze v_string przenosimy kolejno dane do niepotrzebny  x  y  z. Dane s¹ przenoszone po kolei st¹d koniecznoœæ zmiennej "niepotrzebny"
			stringstream uv_string(ptr_uv_string[uv_index]);
			uv_string >> niepotrzebny >> u >> v;
			stringstream vn_string(ptr_vn_string[nor_index]);
			vn_string >> niepotrzebny >> xn >> yn >> zn;
			
			//konwertujemy skompletowane dane vertexa z typu string na float
			xfloat = stof(x);// funkcja stof konwertuje string na float
			yfloat = stof(y);
			zfloat = stof(z);
			ufloat = stof(u);
			vfloat = stof(v);
			xnfloat = stof(xn);
			ynfloat = stof(yn);
			znfloat = stof(zn);
			
			//sprawdzamy czy takie dane s¹ ju¿ w vectorze dane_vertexow
			for (int idx = 0; idx < zajete_miejsca; idx++)
			{//warunek jest spe³niony gdy dane skompletowanego vertexa s¹ identyczne co , któregoœ z vertexów w vectorze
				int index_wsp_x = idx * 8;
				if (xfloat == dane_vertexow[index_wsp_x] && yfloat == dane_vertexow[index_wsp_x + 1] && zfloat == dane_vertexow[index_wsp_x + 2] &&
					ufloat == dane_vertexow[index_wsp_x + 3] && vfloat == dane_vertexow[index_wsp_x + 4] &&
					xnfloat == dane_vertexow[index_wsp_x + 5] && ynfloat == dane_vertexow[index_wsp_x + 6] && znfloat == dane_vertexow[index_wsp_x + 7])
				{
					juz_jest = true;
					//nie wa¿ne czy dane skompletowanego vertexa siê powtarzaj¹ czy nie, co ka¿d¹ iteracjê zapisujemy index skompletowanego vertexa 
					wskaznik_inde[index_kolejnego_indexa] = idx;
					index_kolejnego_indexa++;
					break;
				}
			}

			if (!juz_jest)
			{
				//przenosimy dane skompletowanego vertexa na koniec vectora
				dane_vertexow.push_back(xfloat);
				dane_vertexow.push_back(yfloat);
				dane_vertexow.push_back(zfloat);
				dane_vertexow.push_back(ufloat);
				dane_vertexow.push_back(vfloat);
				dane_vertexow.push_back(xnfloat);
				dane_vertexow.push_back(ynfloat);
				dane_vertexow.push_back(znfloat);

				wskaznik_inde[index_kolejnego_indexa] = index_kolejnego_vertexa;
				index_kolejnego_vertexa++;
				index_kolejnego_indexa++;
				zajete_miejsca++;	
			}
			//rezerwujemy pamiêæ dla danych vertexów dopiero tutaj, poniewa¿ dopiero po sprawdzeniu powtarzalnoœci danych vertexów wiemy ile dok³adnie potrzebujemy pamiêci dla tych danych
			wskaznik_vert = new float[dane_vertexow.size()];
			//przenosimy dane z vectora do tablicy klasy Object
			for (int index = 0; index < dane_vertexow.size(); index++)
			{
				wskaznik_vert[index] = dane_vertexow[index];
			}
			
		}

		delete[] ptr_numery_wierszy_poz;
		delete[] ptr_numery_wierszy_uv;
		delete[] ptr_numery_wierszy_norm;
		delete[] ptr_v_string;
		delete[] ptr_uv_string;
		delete[] ptr_vn_string;

		file.seekg(0);
		file.close();
		
		liczba_bajtow_do_renderu = dane_vertexow.size() * 4;//4 bo float i int maj¹ po 4 bajty
		liczba_bajtow_indexow = liczba_inde * 4;
		
	}

	void Load_Texture(const char *fname, int id)//wczytywanie tekstury
	{
		glGenTextures(1, &textures[id]);
		tex[id] = &textures[id];

		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width = 0, height = 0, nrChannels = 0;
		unsigned char *data = stbi_load(fname, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void draw_mesh(int id)
	{
		
		glBufferData(GL_ARRAY_BUFFER, liczba_bajtow_do_renderu, wskaznik_vert, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, liczba_bajtow_indexow, wskaznik_inde, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glDrawElements(GL_TRIANGLES, liczba_inde, GL_UNSIGNED_INT, 0);
		
	}
};

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//utworzenie trybu full screen
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Project", glfwGetPrimaryMonitor(), NULL);
	//dane dla mouse_callback
	lastX = (float)mode->width / 2.0;
	lastY = (float)mode->height / 2.0;
	
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	
	//ustawiamy funkcjê mouse_callback dla okna window, ¿eby funkcja mouse_callback by³a uruchamiana w przypadku ruchu kursora
	glfwSetCursorPosCallback(window, mouse_callback);
	//kursor staje siê niewidoczny w czasie trwania programu
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	

	int vertexShader = Create_Vertex_Shader("shaders/vertex_shader.vs");
	int fragmentShader = Create_Fragment_Shader("shaders/fragment_shader.fs");
	int Program = Create_Program(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	Object grass(0);
	grass.load_objn_mesh("objects/plane.obj");
	grass.Load_Texture("textures/grass.jpg", 0);
	grass.Load_Texture("textures/red.jpg", 1);

	
	Object hands(0);
	hands.load_objn_mesh("objects/hands.obj");
	hands.Load_Texture("textures/hands.png", 0);

	Object face(0);
	face.load_objn_mesh("objects/face.obj");
	face.Load_Texture("textures/face.png", 0);

	Object sphere(0);
	sphere.load_objn_mesh("objects/sphere.obj");
	sphere.Load_Texture("textures/sphere.png", 0);
	
	Object cube_test(0);
	cube_test.load_objn_mesh("objects/cube.obj");
	cube_test.Load_Texture("textures/cube.png", 0);
	
	//bufor VAO przechowuje informacje: sk¹d pobraæ odpowiednie dane z pamiêci GPU i dok¹d je przes³aæ oraz w jaki sposób 
	//bufor VBO przechowuje dane vertexów (pozycje, uv, wektory normalne)
	//bofor EBO przechowuje indexy vertexów
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//mówimy programowi, ¿e bêdziemy u¿ywaæ interpretacji danych zawartych w VAO
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	//okreœlamy interpretacjê dla vertexów, uv i normalnych
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));//vertexy
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));//uv
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));//normalne
	glEnableVertexAttribArray(2);

	//pobieranie lokalizacji z "Program" dla "model", "view" itd..
	int modelloc = glGetUniformLocation(Program, "model");
	int viewloc = glGetUniformLocation(Program, "view");
	int projectionloc = glGetUniformLocation(Program, "projection");
	int textureloc = glGetUniformLocation(Program, "ourTexture");
	int lightDiffuseloc = glGetUniformLocation(Program, "lightColor");
	int lightPosloc = glGetUniformLocation(Program, "lightPos");
	int viewPosloc = glGetUniformLocation(Program, "viewPos");
	
	glUseProgram(Program);
	//kolor œwiat³a
	glUniform3f(lightDiffuseloc, 1.0f, 1.0f, 1.0f);
	
	
	
	//rysowanie œcian, których vertexy ustawione s¹ w kolejnoœci przeciwnej do ruchu wskazówek zegara 
	glEnable(GL_CULL_FACE);
	//w³¹czamy test g³êbokoœci
	glEnable(GL_DEPTH_TEST);
	//w³¹czamy blending
	glEnable(GL_BLEND);
	//zarówno dla source jak i dla destination blend factor wynosi 1,1,1,1 (w rezultacie wartoœci tekstur zostan¹ dodane)
	glBlendFunc(GL_ONE, GL_ONE);
	//macierz odpowiedzialna za translacje, rotacjê i skalowanie obiektów
	glm::mat4 model;
	//macierz odpowiedzialna za "przeniesienie" obiektów z uk³adu œwiatowego do uk³adu kamery
	glm::mat4 view;
	//macierz odpowiedzialna za projekcjê perspektywistyczn¹
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(fov), (float)mode->width / (float)mode->height, 0.1f, 10000.0f);
	
	while (!glfwWindowShouldClose(window))
	{
	
		processInput(window);
		//odœwie¿anie okna
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			
		model = glm::mat4(1.0f);
		//przeskalowanie plane'a
		model = glm::scale(model, glm::vec3(10, 10, 10));

		view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//przesy³anie macierzy dla obiektu 
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionloc, 1, GL_FALSE, &projection[0][0]);
		
		//przesy³amy pozycjê œwiat³a i pozycjê kamery co klatkê, poniewa¿ mo¿emy je zmieniaæ co klatkê co wymaga aktualizacji danych
		glUniform3f(lightPosloc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosloc, cameraPos.x, cameraPos.y, cameraPos.z);
		
		//w³¹czamy blending
		glEnable(GL_BLEND);
		//rysowanie grass z tekstur¹ grass
		grass.draw_mesh(0);
		//z tej racji, ¿e plane jest tym samym obiektem z dwoma teksturami, które siê blenduj¹
		//ustawiamy, ¿e test g³êbokoœci przechodzi zawsze
		glDepthFunc(GL_ALWAYS);
		//rysowanie grass z tkstur¹ red
		grass.draw_mesh(1);
		//po blendingu pod³o¿a ustawiamy domyœlny test g³êbokoœci
		glDepthFunc(GL_LESS);
		//wy³¹czamy blending, poniewa¿ chcemy go tylko dla pod³o¿a
		glDisable(GL_BLEND);
		
		model = glm::mat4(1.0f);
		//przesy³amy macierz model jako macierz identycznoœci, poniewa¿ nie chcemy skalowaæ nastêpnych obiektów
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, &model[0][0]);
		
		hands.draw_mesh(0);
		face.draw_mesh(0);	
		sphere.draw_mesh(0);	
	
		model = glm::mat4(1.0f);	
		//skalowanie cube'a
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));
		//translacja cube'a
		model = glm::translate(model, glm::vec3(5, 0, 0));
		//wysy³amy macierz model dla cube'a
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, &model[0][0]);
		cube_test.draw_mesh(0);
	
		//zamiana back bufora z front buforem
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//usuwanie buforów
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);	
	glDeleteBuffers(1, &EBO);
	
	glfwTerminate();
	return 0;
}











