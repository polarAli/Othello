#include <iostream>
#include <vector>


#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

enum colors {
    blank, black, white
};

struct Point {
    int x;
    int y;
    colors color{blank};

    void invert();

    colors inverted_color();
};

void Point::invert() {
    color = inverted_color();
}

colors Point::inverted_color() {
    if (color == black)
        return white;
    else if (color == white)
        return black;
    return blank;
}

struct Player {
    string username;
    colors color{black};
    vector<Point> valid_moves{};
};

struct Board {
    vector<vector<Point>> points{};
    vector<Player> players{};
    int width{8};
    bool is_finished{false};
    Player winner{};

    void ready();

    void get_players();

    void start();

    void end();

    void print(Player);

    void print_result();

    void set_point(int, int, Player &);

    void check_vertical(int);

    void check_horizontal(int);

    void check_cross(int, int);

    vector<Point> get_valid_moves(colors);

    static bool is_valid(const Player &, Point);

private:
    void perform_change(int, int);
};


int main() {
    Board game_board;
    game_board.ready();
    game_board.get_players();
    game_board.start();
    return 0;
}

void Board::ready() {
    for (int i = 0; i < width; ++i) {
        vector<Point> row;
        for (int j = 0; j < width; ++j) {
            Point p;
            p.x = j;
            p.y = i;
            p.color = blank;
            row.push_back(p);
        }
        points.push_back(row);
    }
    Player p1 = Player(), p2 = Player();
    players.push_back(p1);
    players.push_back(p2);
}

void Board::get_players() {
    for (auto &player:players) {
        cout << "Please enter username: ";
        cin >> player.username;
    }
    players.at(1).color = white;
}

void Board::start() {
    points.at(3).at(3).color = white;
    points.at(4).at(4).color = white;
    points.at(3).at(4).color = black;
    points.at(4).at(3).color = black;

    players.at(0).valid_moves = get_valid_moves(players.at(0).color);
    players.at(1).valid_moves = get_valid_moves(players.at(1).color);

    print(players.at(0));

    while (!is_finished) {
        for (auto &player:players) {
            if (!player.valid_moves.empty()) {
                print(player);
                int x, y;
                cout << player.username << " Please enter your next move: ";
                cin >> x >> y;
                x--, y--;
                while (!is_valid(player, points.at(y).at(x))) {
                    cout << "Wrong input!\n";
                    cout << player.username << " Please enter your next move: ";
                    cin >> x >> y;
                    x--, y--;
                }
                set_point(x, y, player);
                is_finished = players.at(0).valid_moves.empty() && players.at(1).valid_moves.empty();
            }
        }
    }

    end();
}

void Board::print(Player player) {
    system(CLEAR);
    cout << "     1   2   3   4   5   6   7   8 \n";
    cout << "   +---+---+---+---+---+---+---+---+\n";
    for (int i = 0; i < width; ++i) {
        cout << " " << i + 1 << " |";
        for (int j = 0; j < width; ++j) {
            cout << " ";
            Point this_point = points.at(i).at(j);
            if (this_point.color == black)
                cout << "B";
            else if (this_point.color == white)
                cout << "W";
            else {
                if (is_valid(player, this_point))
                    cout << ".";
                else
                    cout << " ";
            }
            cout << " |";
        }
        cout << "\n";
        cout << "   +---+---+---+---+---+---+---+---+\n";
    }
    cout << "\n\n" << players.at(0).username << ": B\t" << players.at(1).username << ": W\n";
}

void Board::print_result() {
    cout << "Winner is " << winner.username;
}

void Board::set_point(int x, int y, Player &player) {
    points.at(y).at(x).color = player.color;
    perform_change(x, y);
    for (auto &p:players)
        p.valid_moves = get_valid_moves(p.color);
}

void Board::perform_change(int x, int y) {
    check_vertical(x);
    check_horizontal(y);
    check_cross(x, y);
}

void Board::check_vertical(int x) {
    Point first_point = points.at(0).at(x);
    for (int i = 0; i < width; ++i) {
        Point this_point = points.at(i).at(x);
        if (this_point.color != blank) {
            if (first_point.color == blank) {
                first_point = this_point;
                continue;
            }
            if (first_point.color == this_point.color) {
                for (int j = first_point.x + 1; j < i; ++j) {
                    points.at(j).at(x).invert();
                }
            }
        } else {
            first_point = this_point;
        }
    }
}

void Board::check_horizontal(int y) {
    Point first_point = points.at(y).at(0);
    for (int i = 0; i < width; ++i) {
        Point this_point = points.at(y).at(i);
        if (this_point.color != blank) {
            if (first_point.color == blank) {
                first_point = this_point;
                continue;
            }
            if (first_point.color == this_point.color) {
                for (int j = first_point.x + 1; j < i; ++j) {
                    points.at(y).at(j).invert();
                }
                first_point = this_point;
            }
        } else {
            first_point = this_point;
        }
    }
}

void Board::check_cross(int x, int y) {
    Point start_point = points.at(y).at(x);
    for (int i = x + 1, j = y - 1; i < width && j >= 0; ++i, --j) {
        Point this_point = points.at(j).at(i);
        if (this_point.color == blank)
            continue;
        if (this_point.color == start_point.color) {
            for (int k = x + 1, z = y - 1; k < i && z > j; ++k, --z) {
                points.at(z).at(k).invert();
            }
            break;
        }
    }
    for (int i = x + 1, j = y + 1; i < width && j < width; ++i, ++j) {
        Point this_point = points.at(j).at(i);
        if (this_point.color == blank)
            break;
        if (this_point.color == start_point.color) {
            for (int k = x + 1, z = y + 1; k < i && z < j; ++k, ++z) {
                points.at(z).at(k).invert();
            }
            break;
        }
    }
    for (int i = x - 1, j = y + 1; i >= 0 && j < width; --i, ++j) {
        Point this_point = points.at(j).at(i);
        if (this_point.color == blank)
            break;
        if (this_point.color == start_point.color) {
            for (int k = x - 1, z = y + 1; k > i && z < j; --k, ++z) {
                points.at(z).at(k).invert();
            }
            break;
        }
    }
    for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
        Point this_point = points.at(j).at(i);
        if (this_point.color == blank)
            break;
        if (this_point.color == start_point.color) {
            for (int k = x - 1, z = y - 1; k > i && z > j; --k, --z) {
                points.at(z).at(k).invert();
            }
            break;
        }
    }
}

vector<Point> Board::get_valid_moves(colors color) {
    vector<Point> valid_points;
    vector<Point> same_color_points;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < width; ++j) {
            if (points.at(i).at(j).color == color)
                same_color_points.push_back(points.at(i).at(j));
        }
    }
    for (auto &point : same_color_points) {
        bool have_neighbor = false;
        for (int i = point.x + 1; i < width; ++i) {
            if (points.at(point.y).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(point.y).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(point.y).at(i));
                break;
            }
        }
        have_neighbor = false;
        for (int i = point.x - 1; i > 0; --i) {
            if (points.at(point.y).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(point.y).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(point.y).at(i));
                break;
            }
        }
        have_neighbor = false;
        for (int j = point.y - 1; j > 0; --j) {
            if (points.at(j).at(point.x).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(point.x).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(point.x));
                break;
            }
        }
        have_neighbor = false;
        for (int j = point.y + 1; j < width; ++j) {
            if (points.at(j).at(point.x).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(point.x).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(point.x));
                break;
            }
        }
        have_neighbor = false;
        for (int i = point.x + 1, j = point.y - 1; i < width && j >= 0; ++i, --j) {
            if (points.at(j).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(i));
                break;
            }
        }
        have_neighbor = false;
        for (int i = point.x + 1, j = point.y + 1; i < width && j < width; ++i, ++j) {
            if (points.at(j).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(i));
                break;
            }
        }
        have_neighbor = false;
        for (int i = point.x - 1, j = point.y - 1; i >= 0 && j >= 0; --i, --j) {
            if (points.at(j).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(i));
                break;
            }
        }
        have_neighbor = false;
        for (int i = point.x - 1, j = point.y + 1; i >= 0 && j < width; --i, ++j) {
            if (points.at(j).at(i).color == point.inverted_color()) {
                have_neighbor = true;
            } else {
                if (points.at(j).at(i).color == blank && have_neighbor)
                    valid_points.push_back(points.at(j).at(i));
                break;
            }
        }
    }
    return valid_points;
}

bool Board::is_valid(const Player &player, Point move) {
    for (auto point:player.valid_moves)
        if (move.x == point.x && move.y == point.y)
            return true;
    return false;
}

void Board::end() {
    int player1_sum = 0, player2_sum = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < width; ++j) {
            if (points.at(i).at(j).color == players.at(0).color)
                player1_sum++;
            else if (points.at(i).at(j).color == players.at(1).color)
                player2_sum++;
        }
    }
    if (player1_sum > player2_sum)
        winner = players.at(0);
    else
        winner = players.at(1);
}

