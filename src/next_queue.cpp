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

std::queue<Tetramino> NextQueue::asQueue() {
    std::queue<Tetramino> result;
    std::queue<Tetramino> temp_bag = bag;
    std::queue<Tetramino> temp_next_bag = next_bag;

    // Merge q1 into result
    while (!temp_bag.empty()) {
        result.push(temp_bag.front());
        temp_bag.pop();
    }

    // Merge q2 into result
    while (!temp_next_bag.empty()) {
        result.push(temp_next_bag.front());
        temp_next_bag.pop();
    }

    return result;

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

void NextQueue::draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size){
    //TODO: center all pieces 
    std::queue<Tetramino> cur_queue = asQueue();
    int i = 0;
    int gap = 10;

    while (!cur_queue.empty()){
        Tetramino* cur_piece = &cur_queue.front();

        auto [min_x, min_y, w, h] = cur_piece->getBounds();
        auto [cx, cy] = cur_piece->getCenter();
        int x0 = dx - cx;
        int y0 = dy + (min_y + h - cur_piece->getShape()[0].size()) * tile_size;
        cur_piece->drawOffBoard(window, x0, y0 + gap);
        gap += h * tile_size + 10;
        cur_queue.pop();
        if (++i == 5) return; // only draw 5
    }
}