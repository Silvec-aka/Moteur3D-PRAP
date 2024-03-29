#include "engine3D.hpp"
#include "matrix.hpp"

Engine3D::Engine3D(Scene3D& _scene, const int _window_width, const int _window_height, const float _fov_factor)
{
	scene = _scene;
    window_width = _window_width;
    window_height = _window_height;
    fov_factor = _fov_factor;
    running = true;

    //on initialise sdl
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        running = false;
        return;
	}

	//on créé la fenêtre
	window = SDL_CreateWindow(
        "par Jules Pellevoizin et Alec Borel",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        0
    );

	//on retourne une erreur si la fenêtre n'est pas créé
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        running = false;
        return;
    }
    
    // on crée l’objet qui affichera la scène
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        running = false;
        return;
    }
    // la couleur noire pour le fond de la fenêtre
    SDL_SetRenderDrawColor(renderer, 0, 0, 0,SDL_ALPHA_OPAQUE);

    // on efface la fenêtre
    SDL_RenderClear(renderer);
    //on l'affiche
    SDL_RenderPresent(renderer);
}

void Engine3D::Setrunning(bool _running)
{
	running = _running;
}

void Engine3D::render(float time, bool isAnimated)
{
    // on efface la fenêtre
    SDL_SetRenderDrawColor(renderer, 0, 0, 0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

	std::vector<Mesh3D*> meshs = scene.getMeshs(); 

	//On récupère les faces de chaque mesh
    std::vector<std::vector<Quad3D>> faces;
    for (int i=0; i< (int) meshs.size(); i++)
    {
        faces.push_back(meshs[i]->get_Quads());
    }
    
    //On récupère les triangles de chaque face
    std::vector<Triangle3D> triangles;
    for (int i=0; i< (int) faces.size(); i++)
    {
        for (int j=0; j< (int) faces[i].size(); j++)
        {
            triangles.push_back(faces[i][j].get_trig1());
            triangles.push_back(faces[i][j].get_trig2());
        }
    }
    //Défintion de la matrice (est normalisée donc renvoie toujours un résultat entre -1 et 1)
    Matrix matProj;
    matProj.initializeProj(window_width, window_height);
    //matrices de rotation
    Matrix matRotZ, matRotX, matRotY;


    //valeur de rotation
    float fTheta;
    if (isAnimated) fTheta = time;
    else fTheta=0;

    matRotZ.initializeZRot(fTheta);
    matRotX.initializeXRot(fTheta);
    matRotY.initializeYRot(fTheta);

    Vector3D cameraPos = scene.getCameraPosition();

    //stockage des triangles projetés
    std::vector<Triangle3D> triangleProjs;

    for (auto tri : triangles)
    {
        Triangle3D newTriangle = Triangle3D(tri); 
        newTriangle.set_i(tri.get_i());

        //on applique la rotation au triangle
        newTriangle.multiplyByMatrix(matRotY);
        // newTriangle.multiplyByMatrix(matRotZ);

        //calcul de la normal d--u triangle
        Vector3D normaltri, line1, line2;
        line1.set_x(newTriangle.get_b().get_x() - newTriangle.get_a().get_x());
        line1.set_y(newTriangle.get_b().get_y() - newTriangle.get_a().get_y());
        line1.set_z(newTriangle.get_b().get_z() - newTriangle.get_a().get_z());

        line2.set_x(newTriangle.get_c().get_x() - newTriangle.get_a().get_x());
        line2.set_y(newTriangle.get_c().get_y() - newTriangle.get_a().get_y());
        line2.set_z(newTriangle.get_c().get_z() - newTriangle.get_a().get_z());

        normaltri = CrossProduct(line1,line2);
        normaltri.normalize();

        Vector3D cameraRay = newTriangle.get_b()-cameraPos;
        cameraRay.normalize();

        float res =   normaltri.get_x()*cameraRay.get_x() 
                    + normaltri.get_y()*cameraRay.get_y() 
                    + normaltri.get_z()*cameraRay.get_z();
        
        if (res<=0)
        {
            // Illumination
            Vector3D light_direction = Vector3D(0.0f, -0.5f, -2.0f);
            float intensite = 5.0f;
            
            Vector3D lightRay = newTriangle.getCenterOfThirdSide() - light_direction;
            float lightRayMagnitude = lightRay.magnitude();
            float dot_l = normaltri.dotProduct(lightRay);
            float theta = std::acos(dot_l / (normaltri.magnitude() * lightRay.magnitude()));      //E=(I/d^2)*cos(theta) = formule de l'éclairemnt      
            float eclairement = intensite / pow(lightRayMagnitude,2) * cos(theta);
            if(eclairement<0.1) eclairement= 0.1;
            if(eclairement>1) eclairement= 1;

            newTriangle.set_i(eclairement);

            newTriangle.multiplyByMatrix(matProj);

            newTriangle.scaleToViewAndWindow(window_width, window_height);

            triangleProjs.push_back(newTriangle);
        }
        std::sort(triangleProjs.begin(), triangleProjs.end(), [](Triangle3D &t1, Triangle3D &t2)
        {
            float z1 = (t1.get_a().get_z() + t1.get_b().get_z() + t1.get_c().get_z()) / 3.0f;
            float z2 = (t2.get_a().get_z() + t2.get_b().get_z() + t2.get_c().get_z()) / 3.0f;
            return z1 < z2;
        });     
    }
for (auto &triProjected : triangleProjs){
        SDL_Point A = {(int) triProjected.get_a().get_x(),(int) triProjected.get_a().get_y()};
        SDL_Point B = {(int) triProjected.get_b().get_x(),(int) triProjected.get_b().get_y()};
        SDL_Point C = {(int) triProjected.get_c().get_x(),(int) triProjected.get_c().get_y()};
           
        drawTriangle(A, B, C, triProjected.get_i());
    }
SDL_RenderPresent(renderer);
}

bool Engine3D::is_running()
{
    return running;
}

void Engine3D::destroy_window()
{
    SDL_RenderClear(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Engine3D::fillTriangle(SDL_Point v1, SDL_Point v2, SDL_Point v3)
{
    
    // Trier les sommets du triangle par ordre croissant de y
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v1.y > v3.y) std::swap(v1, v3);
    if (v2.y > v3.y) std::swap(v2, v3);
    // Déterminer la zone de remplissage du triangle
    int total_height = v3.y - v1.y;
    for (int i = 0; i < total_height; i++)
    {
        bool second_half = i > v2.y - v1.y || v2.y == v1.y;
        int segment_height = second_half ? v3.y - v2.y : v2.y - v1.y;
        float alpha = (float) i / total_height;
        float beta  = (float)(i - (second_half ? v2.y - v1.y : 0)) / segment_height;
        SDL_Point A = {v1.x + (int)(alpha * (v3.x - v1.x)), v1.y + i};
        SDL_Point B = {second_half ? v2.x + (int)(beta * (v3.x - v2.x)) : v1.x + (int)(beta * (v2.x - v1.x)), v1.y+i};
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++)
        {
            SDL_RenderDrawPoint(renderer, j, A.y);;
        }
    }
}

void Engine3D::drawTriangle(SDL_Point v1, SDL_Point v2, SDL_Point v3, float eclairement){   //ça marche
    //Couleur de remplissage
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
    SDL_SetRenderDrawColor(renderer,color.r * eclairement, color.g * eclairement, color.b * eclairement, color.a); //ombre
    //remplie le triangle
    fillTriangle(v1,v2,v3);

    SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE);
    int line_thickness = 1;
        for (int i = 0; i < line_thickness; i++)
        {   
            SDL_RenderDrawLine(renderer, v1.x + i, v1.y, v2.x + i, v2.y);
            SDL_RenderDrawLine(renderer, v2.x + i, v2.y, v3.x + i, v3.y);
        }
}
