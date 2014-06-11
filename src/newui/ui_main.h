// ui_main.h
//

class idUIButton {
public:
				idUIButton( const char *path, int x, int y, int width, int height );


	void		Draw( void );

	bool		IsPointInsideButton( int x, int y );
private:
	int			_x;
	int			_y;
	int			_width;
	int			_height;
	idImage		*image;
};

class idNewHud {
public:
					idNewHud();

	void			Draw( void );

	void			OnPressEvent( int x, int y );
	void			OnPressRelease( int x, int y );
private:
	idUIButton		*upButton;
	idUIButton		*downButton;

	idUIButton		*leftButton;
	idUIButton		*rightButton;

	idUIButton		*fireButton;
	//idUIButton		*lel;
};