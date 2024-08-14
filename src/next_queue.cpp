#include "../includes/next_queue.h"
#include <random>
#include "../includes/board.h"

NextQueue::NextQueue(){}

void NextQueue::init(Board* _board){
    board = _board;
    bag = createBag();
    next_bag = createBag();
}

std::queue<Tetramino> NextQueue::createBag(){
    std::queue<Tetramino> new_bag{};
    std::vector<Tetramino> pieces{
        createI(board),
        createJ(board), 
        createL(board), 
        createO(board), 
        createS(board), 
        createT(board), 
        createZ(board)
    };

    // shuffle pieces
    std::random_device rd;
    std::mt19937 g(rd());
    
    std::shuffle(pieces.begin(), pieces.end(), g);

    for (Tetramino& piece : pieces){
        new_bag.push(piece);
    }

    return new_bag;
}

Tetramino NextQueue::pop(){
    if (bag.empty()){
        bag = next_bag;
        next_bag = createBag();
    }

    Tetramino piece = bag.front();
    bag.pop();
    return piece;
}

void NextQueue::draw(sf::RenderWindow* window, int board_x, int board_y){
    //TODO: center all pieces 
    std::queue<Tetramino> temp_bag = bag;
    std::queue<Tetramino> temp_next_bag = next_bag;
    int i = 0;
    const int tile_size = board->tile_size;
    int gap = 10;

    while (!temp_bag.empty()){
        Tetramino* cur_piece = &temp_bag.front();

        auto [min_x, min_y, w, h] = cur_piece->getBounds();
        auto [cx, cy] = cur_piece->getCenter();
        unsigned int x0 = board_x + tile_size * (board->getWidth() + 2) - cx + 10;
        unsigned int y0 = board_y + (min_y + h - cur_piece->getShape()[0].size()) * tile_size;
        cur_piece->drawOffBoard(window, x0, y0 + gap);
        gap += h * tile_size + 10;
        temp_bag.pop();
        if (++i == 5) return;
    }
    while (!temp_next_bag.empty()){
        Tetramino* cur_piece = &temp_next_bag.front();

        auto [min_x, min_y, w, h] = cur_piece->getBounds();
        auto [cx, cy] = cur_piece->getCenter();
        unsigned int x0 = board_x + tile_size * (board->getWidth() + 2) - cx + 10;
        unsigned int y0 = board_y + (min_y + h - cur_piece->getShape()[0].size()) * tile_size;
        cur_piece->drawOffBoard(window, x0, y0 + gap);
        gap += h * tile_size + 10;
        temp_next_bag.pop();
        if (++i == 5) return;
    }
}