#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

//thank you reddit for the improvements

constexpr char YES = 'y';
constexpr char NO = 'n';
constexpr int BLACKJACK = 21;

constexpr int STAND = 17;
constexpr int DEALER_THRESHOLD = 90;

const std::string CLUBS = "Clubs";
const std::string DIAMONDS = "Diamonds";
const std::string HEARTS = "Hearts";
const std::string SPADES = "Spades";


const std::string BLACKJACK_ASCII = R"( 
 _     _            _    _            _    
| |   | |          | |  (_)          | |   
| |__ | | __ _  ___| | ___  __ _  ___| | __
| '_ \| |/ _` |/ __| |/ / |/ _` |/ __| |/ /
| |_) | | (_| | (__|   <| | (_| | (__|   < 
|_.__/|_|\__,_|\___|_|\_\ |\__,_|\___|_|\_\
                       _/ |                
                      |__/)";

class Card {
public:

	Card() = default;
	Card(std::string suit, int value) : suit{ suit }, value{value} {};


	int get_value() const {
		return this->value;
	}

	friend std::ostream& operator << (std::ostream& os, Card& card) {
		os << card.suit << " " << card.value;
		return os;
	};

	

private:
	std::string suit;
	int value;


};

class Deck {
public:
	Deck() {


		const int val[13] = {2,3,4,5,6,7,8,9,10,10,10,10,11};
		const std::string suits[4] = { DIAMONDS, SPADES, HEARTS, CLUBS };

		for (const std::string suit: suits) {
			for (const int &value : val) {
				this->deck.push_back(Card(suit, value));
			}
		}
		std::random_device rd;
		std::mt19937 g{ rd() };

		std::shuffle(this->deck.begin(), this->deck.end(), g);
	
	};

	std::vector<Card>& get_deck() { return this->deck; };

	void print_deck() {
		for (Card& card : deck) {
			std::cout << card << "\n";
		}
	}
	Card give_card() {
		Card card = this->deck.front(); // not a fan of the !DNRY
		this->deck.erase(this->deck.begin());
		return card;
	}

	size_t size() const { return this->deck.size(); };

private:

	std::vector<Card> deck;

};

class Entity {
public:
	std::vector<Card> hand;

	void push(const Card& card) {
		this->hand.push_back(card);
	}

	int get_score() {
		this->score = 0;

		if (this->hand.empty()) return score;

		for (auto& card : this->hand) {
			this->score += card.get_value();
		}

		this->ace_logic();

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
	int score = 0; // might be better of being local? perhaps not...
	int ace_logic() {
		int ace_count = 0;
		for (auto& card : this->hand) {
			if (card.get_value() == 11) {
				++ace_count;
			}
		}
		
		while (ace_count > 0 && this->score > 21) {
			this->score -= 10; // check the score
			--ace_count; // decrement the ace count exit loop

			//https://stackoverflow.com/questions/46752965/c-blackjack-stuck-trying-to-program-ace thank you ?
		}

		return this->score;

	}

};

class Player: public Entity {
public:

	int score = 100; // should be private

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
			case NO: return false;
			default:
				std::cout << "\nWould you like to hit? y/n " << "\n";
				continue;
			}
		}
	}
};

class Dealer: public Entity {
public:
	

	void hit(Deck& deck) {
		int score = this->get_score();
		
		while(score != BLACKJACK && score < 19 && score != STAND ) {
			this->hand.push_back(deck.give_card());

			std::uniform_int_distribution<int> distribution(1, 100); // may be better off as variables? 1 - 100 range
			std::random_device rd;

			std::mt19937 engine(rd()); // Mersenne twister MT19937 no idea?

			int value = distribution(engine); // engine distributes numbers and selects one
			if (value < DEALER_THRESHOLD && score > STAND && score != BLACKJACK) { 
				this->hand.push_back(deck.give_card());


			}
			score = this->get_score();
		}

		return;
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
};

class BlackJack {
public:
	BlackJack() = default;

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

		while (this->player.hit()) {
			this->player.push(this->deck.give_card());
			this->player.print_hand();
		}


		this->dealer.hit(this->deck);
		this->player.print_hand();
		this->dealer.print_hand();
		this->win_logic();
	}

private:

	void win_logic() {

		std::cout << "\n";

		int dealer_score = this->dealer.get_score();
		int player_score = this->player.get_score();

		if (dealer_score == BLACKJACK) {
			std::cout << "Dealer Blackjack\n";
		}

		if (dealer_score > player_score && dealer_score < BLACKJACK || player_score > BLACKJACK) {
			std::cout << "Dealer Wins\n";
		}

		if (player_score == BLACKJACK) {
			std::cout << "Blackjack\n";
		}

		if (player_score > dealer_score && player_score < BLACKJACK || dealer_score > BLACKJACK) {
			std::cout << "You Win\n";

		}

		if (player_score == dealer_score || player_score > BLACKJACK && dealer_score > BLACKJACK) {
			std::cout << "Draw\n";
		}


		/*else
			std::cout << "what scenerio even is this?";*/
	}

	Dealer dealer;
	Player player;
};

int main() {
	std::cout << "\n" << BLACKJACK_ASCII << "\n";

	char prompt;
	std::cout << "\nWould you like to play? y/n ";
	while (std::cin >> prompt) {
		BlackJack blackjack;
		switch (prompt) {
		case YES: 
			blackjack.init_game();
			std::cout << "\nWould you like to play? y/n ";

			break;
		case NO:
			return -1;
		default:
			std::cout << "\nWould you like to play? y/n " << "\n";
			break;
		}
	}

	
	
	return 0;
}

