#version 460 core
out vec4 FragColor;

in vec3 Normal;   
in vec2 TexCoords;
in vec3 FragPos;  

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform sampler2D ourTexture;

void main()
{
    //ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse light
    vec3 norm = normalize(Normal);
    //obliczamy wektor między pozycją światła, a pozycją fragmentu i normalizujemy go
    vec3 lightDir = normalize(lightPos - FragPos);
    //obliczamy iloczyn skalarny wektora normalnego i wektora światła, następnie
    //używamy funkcji max, żeby uniknąć ujemnego wyniku iloczynu skalarnego
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular light
    float specularStrength = 0.5;
    //obliczamy wektor między pozycją kamery, a pozycją fragmentu i normalizujemy go
    vec3 viewDir = normalize(viewPos - FragPos);
    //obliczamy wektor odbity
    vec3 reflectDir = reflect(-lightDir, norm);  
    //obliczamy iloczyn skalarny wektora odbitego i wektora z kamery, używamy funkcji max jak powyżej
    //żeby upewnić się, że wynik iloczynu skalarnego nie jest ujemny i wynik podnosimy do potęgi 32 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
   
    //combined light
    vec3 result = (ambient + diffuse + specular) * texture(ourTexture, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
} 


