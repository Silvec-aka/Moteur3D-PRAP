#ifndef TRIANGLE3D_HPP
#define TRIANGLE3D_HPP

#include "vector3D.hpp"

/**
 * @class Triangle3D triangle.hpp
 * @brief Triangle3D est une classe qui représente les futures faces des meshs par des triangles,
 * ils sont représenté par 3 point \a a, \a b, \a c dans le sens trigonométrique.
 * Elle se compose de :
 *      - 4 atttributs privés :
 *          - \a a (Vector3)
 *          - \a b (Vector3)
 *          - \a c (Vector3)
 *          - \a color (SDL_Color)
 *      - 2 constructeurs
 *          - un constructeur pas défaut
 *          _ un constructeur de copie
 *          - un constructeur qui prend 3 Vector3D et 1 SDL_Color en entré
 *      - des getters/setters pour chaques attributs
 *      - 4 méthodes publiques :
 *          - getCenterOfThirdSide()
 *          - multiplyByMatrix()
 *          - inverseXY()
 *          - scaleToViewAndWindow()
*/          
class Triangle3D
{
    private :
        /**
         * @brief \a a est le premier point du triangle.
         * Son type est Vector3D.
        */
        Vector3D a;
        /**
         * @brief \a b est le deuxième point du triangle dans le sens trigonométrique.
         * Son type est Vector3D.
        */
        Vector3D b;
        /**
         * @brief \a c est le troisième point du triangle dans le sens trigonométrique.
         * Son type est Vector3D.
        */
        Vector3D c;
        /**
         * @brief \a color est la couleur du triangle.
         * Son type est SDL_Color.
        */
        SDL_Color color;

    public :
        /**
         * @brief Constructeur par défaut.
        */
        Triangle3D();

        /**
         * @brief Constructeur de copie.
         * 
         * @param \a t (Triangle)
         */
        Triangle3D(const Triangle3D& t);

        /**
         * @brief Constructeur qui créé un triangle à partir de 3 point (dans le sens trigonométrique) 
         * et d'une couleur.
         * 
         * @param \a a (Vector3D)
         * @param \a b (Vector3D)
         * @param \a c (Vector3D)
         * @param \a _co (SDL_Color)
        */
        Triangle3D(Vector3D& a, Vector3D& b, Vector3D& c, const SDL_Color co);

        /**
         * @brief Getter pour l'attribut \a a.
         * 
         * @return L'attribut \a a.
        */
        Vector3D& get_a();

        /**
         * @brief Getter pour l'attribut \a b.
         * 
         * @return L'attribut \a b.
        */
        Vector3D& get_b();

        /**
         * @brief Getter pour l'attribut \a c.
         * 
         * @return L'attribut \a c.
        */
        Vector3D& get_c();

        /**
         * @brief Getter pour l'attribut \a color.
         * 
         * @return L'attribut \a color.
        */
        SDL_Color& get_color();

        /**
         * @brief Setter de \a a.
         * 
         * @param \a _a (Vector3D)
        */
        void set_a(const Vector3D& _a);

        /**
         * @brief Setter de \a b.
         * 
         * @param \a _b (Vector3D)
        */
        void set_b(const Vector3D& _b);

        /**
         * @brief Setter de \a c.
         * 
         * @param \a _c (Vector3D)
        */
        void set_c(const Vector3D& _c);

        /**
         * @brief Setter de \a color.
         * 
         * @param \a _color (SDL_Color)
        */
        void set_color(const SDL_Color& _color);

        /**
         * @brief Calcule le centre du côté AC du triangle, pourra être la diagonale d'un Quad3D.
         * 
         * @return Le milieu du côté AC (Vector3D)
        */
        Vector3D getCenterOfThirdSide();

        /**
         * @brief Multiplie les 3 points du triangle par la matrice \a mat.
         * 
         * @param \mat (Matrix)
         * @return Triangle3D
        */
        Triangle3D multiplyByMatrix(const Matrix mat);

        /**
         * @brief Inverse les coordonnées x et y des 3 points du triangle.
        */
        void inverseXY();

        /**
         * @brief Transforme les coordonnées pour qu'elles soient affichables dans une fenêtre.
        */
        void scaleToViewAndWindow(int window_w,int window_ht);

        /**
         * @brief Additionne le vecteur \a vect au trois points du triangle.
        */
        void operator+=(const Vector3D vect);

        /**
         * @brief Multiplie le floattant \a a au coordonnées des trois points du triangles.
        */
        void operator*=(float a);

        Triangle3D operator=(const Triangle3D& t);


};


std::ostream& operator<<(std::ostream& os, Triangle3D& t);


#endif