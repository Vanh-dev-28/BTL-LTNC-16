namespace SpaceInvaders
{
    class Renderer;
    class Bullet
    {
    public:
        void update(float dt);

        void render(Renderer &renderer);

        void move();

        void shoot();

    private:
        float bulletX;
        float bulletY;
    };
}