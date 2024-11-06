import random
import numpy as np
import tensorflow as tf
from tensorflow.keras.layers import Dense, Flatten
from tensorflow.keras.models import Sequential 

# Constants (same as in C code)
BOARD_EMPTY = 0
BOARD_CROSS = 1
BOARD_NOUGHT = 2
PLAYER_1 = 0
PLAYER_2 = 1
AI = -1
UNASSIGNED = -99

class GameState:
    def __init__(self):
        self.board = [[BOARD_EMPTY for _ in range(3)] for _ in range(3)]
        self.player1StartFirst = random.choice([True, False])
        self.isDraw = False
        self.winner = UNASSIGNED
        self.turn = PLAYER_1 if self.player1StartFirst else AI

def do_move(board, row, col, player):
    """
    Executes the current player move in column and row.

    Args:
        board: The tic-tac-toe board.
        col: Column of the move.
        row: Row of the move.
        player: The current player.

    Returns:
        bool: True if successful, False if disallowed.
    """
    if board[row][col] != BOARD_EMPTY:
        return False

    insert_char = BOARD_CROSS if player == PLAYER_1 else BOARD_NOUGHT
    board[row][col] = insert_char
    return True

def check_win(board):
    """
    Checks if the game has been won on any win condition.

    Args:
        board: The tic-tac-toe board.

    Returns:
        bool: True if there is a winner, False otherwise.
    """
    # Check rows
    for row in board:
        if row[0] != BOARD_EMPTY and row[0] == row[1] == row[2]:
            return True
    # Check columns
    for col in range(3):
        if board[0][col] != BOARD_EMPTY and board[0][col] == board[1][col] == board[2][col]:
            return True
    # Check diagonals
    if (board[0][0] != BOARD_EMPTY and board[0][0] == board[1][1] == board[2][2]) or \
       (board[0][2] != BOARD_EMPTY and board[0][2] == board[1][1] == board[2][0]):
        return True
    return False

def check_draw(board):
    """
    Checks if the game has no possible ways of winning.

    Args:
        board: The tic-tac-toe board.

    Returns:
        bool: True if the game is a draw, False otherwise.
    """
    for row in board:
        for cell in row:
            if cell == BOARD_EMPTY:
                return False
    return True

def next_turn(game_state):
    """
    Checks for win and draw conditions and updates GameState accordingly.

    Args:
        game_state: The current game state.
    """
    if check_win(game_state.board):
        game_state.winner = game_state.turn
    elif check_draw(game_state.board):
        game_state.isDraw = True
    game_state.turn = PLAYER_1 if game_state.turn == AI else AI

def get_random_move(board):
    """
    Gets a random valid move for the AI.

    Args:
        board: The tic-tac-toe board.

    Returns:
        tuple: A tuple representing the row and column of the random move.
    """
    empty_cells = [(i, j) for i in range(3) for j in range(3) if board[i][j] == BOARD_EMPTY]
    return random.choice(empty_cells)

def print_board(board):
    """Prints the tic-tac-toe board."""
    print("  1   2   3")
    for i in range(3):
        print(chr(65 + i), end="")
        for j in range(3):
            cell = " "
            if board[i][j] == BOARD_CROSS:
                cell = "X"
            elif board[i][j] == BOARD_NOUGHT:
                cell = "O"
            print(f" {cell} ", end="")
            if j < 2:
                print("|", end="")
        print()
        if i < 2:
            print("---+---+---")

# TensorFlow Q-learning implementation

def create_model():
    """Creates the TensorFlow Q-learning model."""
    model = Sequential()
    model.add(Flatten(input_shape=(3, 3), name = "input_t"))  # Input shape for the 3x3 board
    model.add(Dense(128, activation='relu'))
    model.add(Dense(64, activation='relu'))
    model.add(Dense(9, activation='linear', name = "output_t"))  # 9 outputs for each possible move
    model.compile(loss='mse', optimizer='adam')
    print(model.summary())
    return model

def get_state(board):
    """Converts the board to a NumPy array suitable for the model."""
    return np.array(board).reshape(1, 3, 3)

def get_action(model, state, epsilon):
    """
    Chooses an action using an epsilon-greedy strategy.

    Args:
        model: The TensorFlow Q-learning model.
        state: The current state of the game.
        epsilon: The exploration rate.

    Returns:
        int: The chosen action (0-8 representing the board cells).
    """
    if random.random() < epsilon:
        # Explore: choose a random action
        return random.randint(0, 8)
    else:
        # Exploit: choose the action with the highest predicted Q-value
        q_values = model.predict(state)
        return np.argmax(q_values[0])


def train_q_learning(model, episodes=5000, gamma=0.5, epsilon=1.0, epsilon_decay=0.999):
    """
    Trains the Q-learning agent against a random agent.

    Args:
        model: The TensorFlow Q-learning model.
        episodes: The number of episodes to train for.
        gamma: The discount factor.
        epsilon: The initial exploration rate.
        epsilon_decay: The rate at which epsilon decays.
    """
    VALID_MOVE_REWARD = 1
    INVALID_MOVE_REWARD = -20
    WIN_REWARD = 100
    LOSE_REWARD = -100
    DRAW_REWARD = 50 # tic tac toe is highly likely to draw, so use a higher draw reward

    for episode in range(episodes):
        reward = 0
        game_state = GameState()
        state = get_state(game_state.board)
        done = False
        while not done:
            if game_state.turn == AI:  # Q-learning agent's turn
                action = get_action(model, state, epsilon)
                row = action // 3
                col = action % 3
                valid_move = do_move(game_state.board, row, col, AI)
            else:  # Random agent's turn
                row, col = get_random_move(game_state.board)
                valid_move = do_move(game_state.board, row, col, PLAYER_1) 

            next_state = get_state(game_state.board)
            reward += VALID_MOVE_REWARD if valid_move else INVALID_MOVE_REWARD  # Reward for valid move, penalty for invalid
            next_turn(game_state)

            if game_state.winner != UNASSIGNED:
                done = True
                if game_state.winner == AI:
                    reward += WIN_REWARD  # Reward for winning
                else:
                    reward += LOSE_REWARD  # Penalty for losing
            elif game_state.isDraw:
                done = True
                reward += DRAW_REWARD  # Small reward for draw

            if game_state.turn == AI:  # Only update Q-learning agent
                # Q-learning update
                target = reward + gamma * np.max(model.predict(next_state)[0]) * (not done)
                target_f = model.predict(state)
                target_f[0][action] = target
                model.fit(state, target_f, epochs=1, verbose=0)

            state = next_state

        epsilon *= epsilon_decay
        if (episode + 1) % 1000 == 0:
            print(f"Episode: {episode + 1}, Epsilon: {epsilon:.4f}")

    # Save the trained model weights
    model.export("weights")

def main_game():
    """Main function to run the tic-tac-toe game."""
    game_state = GameState()
    print("Player 1 Starts First, First Player Always X (Cross)" if game_state.player1StartFirst
          else "AI Starts First, First Player Always X (Cross)")

    while game_state.winner == UNASSIGNED and not game_state.isDraw:
        print_board(game_state.board)
        if game_state.turn == PLAYER_1:
            while True:
                try:
                    move = input("Enter your move (e.g., A1): ").upper()
                    row_def = ['A', 'B', 'C']
                    row = row_def.index(move[0])
                    col = int(move[1]) - 1
                    print(f"${row} ${col}")
                    if 0 <= row <= 2 and 0 <= col <= 2 and game_state.board[row][col] == BOARD_EMPTY:
                        break
                    else:
                        print("Invalid move. Try again.")
                except (IndexError, ValueError):
                    print("Invalid input format. Try again.")
            do_move(game_state.board, row, col, game_state.turn)
        else:  # AI's turn
            print("AI is making a move...")
            ai_row, ai_col = get_random_move(game_state.board)
            do_move(game_state.board, ai_row, ai_col, game_state.turn)
        next_turn(game_state)

    print_board(game_state.board)
    if game_state.isDraw:
        print("Game Over! Draw!")
    else:
        print(f"Game Over! {'You' if game_state.winner == PLAYER_1 else 'AI'} Win!")

if __name__ == "__main__":
    model = create_model()
    train_q_learning(model)