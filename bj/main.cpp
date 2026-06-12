#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

constexpr char YES = 'y';
constexpr char NO = 'n';
constexpr int BLACKJACK = 21;
constexpr int STAND = 17;


class Card {
public:
	Card(std::string face, int value) :
		face{ face }, value{value}{}

	int get_value() const { return this->value; }

	friend std::ostream& operator << (std::ostream& os, const Card& card) {
		os << card.face << " " << card.value;
		return os;
	};


private:
	std::string face;
	int value;

};

class Deck {
public:

	/*enum faces {
		CLUBS='C',
		DIAMONDS='D',
		SPADES='S',
		HEARTS='H'
	};

	enum values {
		TWO = 2,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN,
		KING=10,
		QUEEN=10,
		JACK=10,
		ACE=1,

	};*/

	Deck() = default;

	std::vector<Card>& get_deck() { return this->deck; };

	void init_deck() {
		const std::vector<int> values{ 2,3,4,5,6,7,8,9,10 };
		const std::vector<std::string> faces{ "Clubs", "Diamonds", "Spades", "Hearts" };

		for (auto& face : faces) {
			for (auto& value : values) {
				this->deck.push_back({ face, value });

			}
		}

		std::random_device rd;
		std::mt19937 g{ rd() };

		std::shuffle(this->deck.begin(), this->deck.end(), g);

	}

	void print_deck() {
		for (Card& card : deck) {
			std::cout << card << "\n";
		}
	}

	/*void test() {
		for (auto face = CLUBS; face != HEARTS; ++face) {
			std::cout << face;
		}
	}*/

	Card give_card() {
		Card card = this->deck.front(); // not a fan of the !DNRY
		this->deck.erase(this->deck.begin());
		return card;
	}

	int size() const { return this->deck.size(); };

private:

	std::vector<Card> deck;

};

class Entity {
public:


	std::vector<Card> hand;

	void push(Card card) {
		this->hand.push_back(card);
	}

	int get_score() {
		this->score = 0;

		if (this->hand.empty()) return score;

		for (auto& card : this->hand) {
			this->score += card.get_value();
		}

		return this->score;
	}

	void print_hand() {
		std::cout << "\nYour Cards\n";

		for (Card& card : hand) {
			std::cout << card << "\n";
		}

		std::cout << "Your Score is: " << this->get_score() << "\n";
	}

private:	

	int score = 0;

};

class Player: public Entity {
public:
	bool hit() {

		if (this->get_score() >= 21) {
			return false;
		}

		char prompt;
		std::cout << "\nWould you like to hit? y/n ";
		while (std::cin >> prompt) {
			switch (prompt) {
			case YES: 
				return true;
			case NO:
				return false;
			default:
				std::cout << "\nWould you like to hit? y/n " << "\n";
				continue;
			}
		}
	}

};

class Dealer: public Entity {
public:

	void hit(Card card) {
		int score = this->get_score();
		
		if (score == STAND) {
			return;
		}


		if (score != BLACKJACK && score < STAND) { // bool array to choose from may be more optimal>
			this->hand.push_back(card);
			return;
		}
	}

	void print_hand() {

		std::cout << "\nDealer Cards\n";
		for (Card& card : hand) {
			std::cout << card << "\n";
		}

		std::cout << "Dealer Score is: " << this->get_score() << "\n";
	}

	void reveal_card() {
		std::cout << "\nDealer Card: " << this->hand.front() << "\n";
	}


private:

	bool hit_chance[5] = { true, true, true, true, false };

};

class BlackJack {
public:
	BlackJack() {
		deck.init_deck();
	};

	Deck deck;

	void deal_cards() {
		for (int i = 0; i < 2; ++i) {
			this->dealer.push(deck.give_card());
			this->player.push(deck.give_card());
		}
	}

	void init_game() {
		this->deal_cards();
		this->player.print_hand();

		while (this->player.hit() && this->player.get_score() <= BLACKJACK) {
			this->player.push(this->deck.give_card());
			this->player.print_hand();
		}


		this->dealer.hit(this->deck.give_card());
		this->player.print_hand();
		this->dealer.print_hand();
		this->win_logic();
	}

private:

	void win_logic() {

		std::cout << "\n";

		int dealer_score = this->dealer.get_score();
		int player_score = this->player.get_score();

		if (player_score == BLACKJACK) {
			std::cout << "Blackjack\n";
		}

		if (dealer_score == BLACKJACK) {
			std::cout << "Dealer Blackjack\n";
		}

		else if (player_score > BLACKJACK && dealer_score > BLACKJACK || player_score == dealer_score) {
			std::cout << "DRAW\n";
			//return;
		}
		else if (player_score > BLACKJACK || player_score < dealer_score && dealer_score < BLACKJACK) {
			std::cout << "Dealers wins\n";
		}

		else if (dealer_score > BLACKJACK || dealer_score < player_score && player_score < BLACKJACK) {
			std::cout << "You win\n";
		}
		else
			std::cout << "what scenerio even is this?";
	}

	Dealer dealer;
	Player player;


};


int main() {

	BlackJack blackjack;
	blackjack.init_game();
	

	
	return 0;
}

