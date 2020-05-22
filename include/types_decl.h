/*
 * There are some circular dependencies in class definitions
 * we have to forward declare the classes to resolve them.
 * Note: in this case, since size of class is unknown, only
 * pointer types may be used as members of circularly 
 * dependent classes.
 */
class Scene;
class Model;
class Light;
class Orientation;
class Texture;
class Data;
class ShadowMap;