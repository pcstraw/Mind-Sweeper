#pragma once

nativenamespace{

	enum class Key { A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	LeftControl,
	RightControl,
	Control,
	LeftShirt,
	RightShift,
	Space,
	Enter,
	Escape ,
	n1,n2,n3,n4,n5,n6,n7,n8,n9,n0,
	Dash,
	Equals,
	Alt,
	AltGr,
	Tild,
	Tab,
	Up,Down,Left,Right,
	Plus,Minus,
	Caps,
	Backspace,
	Menu,
	Insert,
	Delete,
	Home,
	End,
	PageUp,
	PageDown,
	NumLock,
	F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12
};

	enum class KeyState {Up, Pressed, Down};

	class Keyboard
	{


	public:
		Keyboard()
		{
			debug = false;
			state = KeyState::Up;
			_presscount = 0;
		}

		Key key;
		KeyState state;
		int scancode;
		int keycode;
		int actionCode;
		int _presscount;
		bool _ispressed;
		bool debug;
		
		bool IsDown(Key Keybutton)
		{
			if (state == KeyState::Down && key == Keybutton)
				return true;
			return false;
		}

		bool IsKeyCodeDown(int code)
		{
			if (state == KeyState::Down && scancode == code)
				return true;
			return false;
		}

		bool IsUp(Key Keybutton)
		{
			if (state == KeyState::Up && key == Keybutton)
				return true;
			return false;
		}

		bool IsPressed(Key Keybutton)
		{
			if (state == KeyState::Pressed && key == Keybutton)
			{
				if (!_ispressed)
				{
					_ispressed = true;
					_presscount++;
					if (_presscount >= 3)
					{
						_presscount = 0;
						return true;
					}
				}
			}
			_ispressed = false;
			return false;
		}

		void OnKeyboard(int Keycode, int Scancode, int Action, int Mods)
		{
			actionCode = Action;
			//if (Scancode == 0)
			//	return;
			switch (Action) 
			{
				case 0: state = KeyState::Up; 
					break;    
				case 1: 
					state = KeyState::Pressed;
					break;
				case 2: state = KeyState::Down;
			}

			scancode = Scancode;
			keycode = Keycode;
			switch (Scancode)
				{
				case 1:
					key = Key::Escape;
					break;
				case 16:
					key = Key::Q;
					break;
				case 17:
					key = Key::W;
					break;
				case 18:
					key = Key::E;
					break;
				case 19:
					key = Key::R;
					break;
				case 20:
					key = Key::T;
					break;
				case 21:
					key = Key::Y;
					break;
				case 22:
					key = Key::U;
					break;
				case 23:
					key = Key::I;
					break;
				case 24:
					key = Key::O;
					break;
				case 25:
					key = Key::P;
					break;
				case 30:
					key = Key::A;
					break;
				case 31:
					key = Key::S;
					break;
				case 32:
					key = Key::D;
					break;
				case 33:
					key = Key::F;
					break;
				case 34:
					key = Key::G;
					break;
				case 35:
					key = Key::H;
					break;
				case 36:
					key = Key::J;
					break;
				case 37:
					key = Key::K;
					break;
				case 38:
					key = Key::L;
					break;
				case 44:
					key = Key::Z;
					break;
				case 45:
					key = Key::X;
					break;
				case 46:
					key = Key::C;
					break;
				case 47:
					key = Key::V;
					break;
				case 48:
					key = Key::B;
					break;
				case 49:
					key = Key::N;
					break;
				case 50:
					key = Key::M;
					break;
				case 28:
					key = Key::Enter;
					break;
				case 328:
					key = Key::Up;
					break;
				case 336:
					key = Key::Down;
					break;
				case 331:
					key = Key::Left;
					break;
				case 333:
					key = Key::Right;
					break;
				case 11:
					key = Key::n0;
					break;
				case 41:
					key = Key::Tild;
					break;
				case 2:
					key = Key::n1;
					break;
				case 3:
					key = Key::n2;
					break;
				case 4:
					key = Key::n3;
					break;
				case 5:
					key = Key::n4;
					break;
				case 6:
					key = Key::n5;
					break;
				case 7:
					key = Key::n6;
					break;
				case 8:
					key = Key::n7;
					break;
				case 9:
					key = Key::n8;
					break;
				case 10:
					key = Key::n9;
					break;
				case 12:
					key = Key::Dash;
					break;
				case 13:
					key = Key::Equals;
					break;
				case 74:
					key = Key::Minus;
					break;
				case 75:
					key = Key::Plus;
					break;
				case 15:
					key = Key::Tab;
					break;
				case 42:
					key = Key::LeftShirt;
					break;
				case 29:
					key = Key::LeftControl;
					break;
				case 56:
					key = Key::Alt;
					break;
				case 312:
					key = Key::AltGr;
					break;
				case 349:
					key = Key::Menu;
					break;
				case 338:
					key = Key::Insert;
					break;
				case 339:
					key = Key::Delete;
					break;
				case 327:
					key = Key::Home;
					break;
				case 335:
					key = Key::End;
					break;
				case 329:
					key = Key::PageUp;
					break;
				case 337:
					key = Key::PageDown;
					break;
				case 325:
					key = Key::NumLock;
					break;
				case 285:
					key = Key::RightControl;
					break;
				case 54:
					key = Key::RightShift;
					break;
				case 58:
					key = Key::Caps;
					break;
				case 14:
					key = Key::Backspace;
					break;
				case 57:
					key = Key::Space;
					break;
				case 59:
					key = Key::F1;
					break;
				case 60:
					key = Key::F2;
					break;
				case 61:
					key = Key::F3;
					break;
				case 62:
					key = Key::F4;
					break;
				case 63:
					key = Key::F5;
					break;
				case 64:
					key = Key::F6;
					break;
				case 65:
					key = Key::F7;
					break;
				case 66:
					key = Key::F8;
					break;
				case 67:
					key = Key::F9;
					break;
				case 68:
					key = Key::F10;
					break;
				case 87:
					key = Key::F11;
					break;
				case 88:
					key = Key::F12;
					break;
				default:
					break;
			}
			if (debug)
			{
				cout << "Scancode: " << Scancode << endl;
				cout << " - Key: " << GetKeyString(key) << endl;
			}
			this;
		}

		string GetKeyString(const Key& key)
		{
			string result = "none";
			switch (key)
			{
			case Key::Escape:
				result = "Escape";
				break;
			case Key::Q:
				result = "Q";
				break;
			case Key::W:
				result = "W";
				break;
			case Key::E:
				result = "E";
				break;
			case Key::R:
				result = "R";
				break;
			case Key::T:
				result = "T";
				break;
			case Key::Y:
				result = "Y";
				break;
			case Key::U:
				result = "U";
				break;
			case Key::I:
				result = "I";
				break;
			case Key::O:
				result = "O";
				break;
			case Key::P:
				result = "P";
				break;
			case Key::A:
				result = "A";
				break;
			case Key::S:
				result = "S";
				break;
			case Key::D:
				result = "D";
				break;
			case Key::F:
				result = "F";
				break;
			case Key::G:
				result = "G";
				break;
			case Key::H:
				result = "H";
				break;
			case Key::J:
				result = "J";
				break;
			case Key::K:
				result = "K";
				break;
			case Key::L:
				result = "L";
				break;
			case Key::Z:
				result = "Z";
				break;
			case Key::X:
				result = "X";
				break;
			case Key::C:
				result = "C";
				break;
			case Key::V:
				result = "V";
				break;
			case Key::B:
				result = "B";
				break;
			case Key::N:
				result = "N";
				break;
			case Key::M:
				result = "M";
				break;
			case Key::Enter:
				result = "Enter";
				break;
			case Key::Up:
				result = "Up";
				break;
			case Key::Down:
				result = "Down";
				break;
			case Key::Left:
				result = "Left";
				break;
			case Key::Right:
				result = "Right";
				break;
			case Key::n0:
				result = "n0";
				break;
			case Key::Tild:
				result = "Tild";
				break;
			case Key::n1:
				result = "n1";
				break;
			case Key::n2:
				result = "n2";
				break;
			case Key::n3:
				result = "n3";
				break;
			case Key::n4:
				result = "n4";
				break;
			case Key::n5:
				result = "n5";
				break;
			case Key::n6:
				result = "n6";
				break;
			case Key::n7:
				result = "n7";
				break;
			case Key::n8:
				result = "n8";
				break;
			case Key::n9:
				result = "n9";
				break;
			case Key::Dash:
				result = "Dash";
				break;
			case Key::Equals:
				result = "Equals";
				break;
			case Key::Minus:
				result = "Minus";
				break;
			case Key::Plus:
				result = "Plus";
				break;
			case Key::Tab:
				result = "Tab"; //see above
				break;
			case Key::LeftShirt:
				result = "LeftShirt";
				break;
			case Key::LeftControl:
				result = "LeftControl";
				break;
			case Key::Alt:
				result = "Alt";
				break;
			case Key::AltGr:
				result = "AltGr";
				break;
			case Key::Menu:
				result = "Menu";
				break;
			case Key::Insert:
				result = "Insert";
				break;
			case Key::Delete:
				result = "Delete";
				break;
			case Key::Home:
				result = "Home";
				break;
			case Key::End:
				result = "End";
				break;
			case Key::PageUp:
				result = "PageUp";
				break;
			case Key::PageDown:
				result = "PageDown";
				break;
			case Key::RightControl:
				result = "RightControl";
				break;
			case Key::RightShift:
				result = "RightShift";
				break;
			case Key::Caps:
				result = "Caps";
				break;
			case Key::Backspace:
				result = "Backspace";
				break;
			case Key::Space:
				result = "Space";
				break;
			case Key::F1:
				result = "F1";
				break;
			case Key::F2:
				result = "F2";
				break;
			case Key::F3:
				result = "F3";
				break;
			case Key::F4:
				result = "F4";
				break;
			case Key::F5:
				result = "F5";
				break;
			case Key::F6:
				result = "F6";
				break;
			case Key::F7:
				result = "F7";
				break;
			case Key::F8:
				result = "F8";
				break;
			case Key::F9:
				result = "F9";
				break;
			case Key::F10:
				result = "F10";
				break;
			case Key::F11:
				result = "F11";
				break;
			case Key::F12:
				result = "F1";
				break;
			default:
				break;
			}
			return result;
		}
	};
}
