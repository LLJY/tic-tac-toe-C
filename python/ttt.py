import random
import numpy as np
import tensorflow as tf
from tensorflow.keras.layers import Dense, Flatten, Dropout
from tensorflow.keras.models import Sequential 
from matplotlib import pyplot as plt

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
    opt = tf.keras.optimizers.Adam(learning_rate=0.0001)
    model.compile(loss='mse', optimizer=opt)
    print(model.summary())
    return model

def get_state(board, playerStartFirst):
    """
    Converts the board to a NumPy array suitable for the model, 
    where 1 represents the opponent and 2 represents the AI.
    """
    new_board = np.array(board)

    # Swap the representation if the player starts first
    if playerStartFirst:
        new_board = np.where(new_board == 1, 3, new_board)  # Temporary value to avoid conflicts
        new_board = np.where(new_board == 2, 1, new_board)
        new_board = np.where(new_board == 3, 2, new_board)

    # Convert to a format suitable for the model
    return new_board.reshape(1, 3, 3)

def get_action(model, state, epsilon, board):
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
        # Explore: choose a random valid move
        print("\033[93mEpsilon value")
        empty_cells = [(i, j) for i in range(3) for j in range(3) if board[i][j] == BOARD_EMPTY]
        row, col = random.choice(empty_cells)
        return row * 3 + col  # Convert to action index
    else:
        # Exploit: choose the action with the highest predicted Q-value
        q_values = model.predict(state)
        return np.argmax(q_values[0])


def train_q_learning(model, episodes=3000, gamma=0.8, epsilon=1.0, epsilon_decay=0.999):
    """
    Trains the Q-learning agent against a random agent.

    Args:
        model: The TensorFlow Q-learning model.
        episodes: The number of episodes to train for.
        gamma: The discount factor.
        epsilon: The initial exploration rate.
        epsilon_decay: The rate at which epsilon decays.
    """
    VALID_MOVE_REWARD = 20
    INVALID_MOVE_REWARD = -100
    WIN_REWARD = 100
    LOSE_REWARD = -100
    DRAW_REWARD = 50 # tic tac toe is highly likely to draw, so use a higher draw reward

    wins = 0
    losses = 0
    draws = 0
    win_history = []  # List to store win counts at intervals
    loss_history = []  # List to store loss counts at intervals
    draw_history = []  # List to store draw counts at intervals

    for episode in range(episodes):
        print(f"Episode {episode} of episodes {episodes}")
        reward = 0
        game_state = GameState()
        state = get_state(game_state.board, game_state.player1StartFirst)
        done = False
        while not done:
            if game_state.turn == AI:  # Q-learning agent's turn
                # action = get_action(model, state, epsilon)
                # row = action // 3
                # col = action % 3
                # valid_move = do_move(game_state.board, row, col, AI)
                valid_move = False
                while(not valid_move):
                    action = get_action(model, state, epsilon, game_state.board)
                    row = action // 3
                    col = action % 3
                    next_state = get_state(game_state.board, game_state.player1StartFirst)
                    valid_move = do_move(game_state.board, row, col, AI)
                    if(not valid_move):
                        print("invalid move")
                    # update the model to teach it valid or invalid moves
                    reward += VALID_MOVE_REWARD if valid_move else INVALID_MOVE_REWARD
                    target = reward + gamma * np.max(model.predict(next_state)[0])
                    print(f"{target}           {reward}")
                    target_f = model.predict(state)
                    target_f[0][action] = reward
                    model.fit(state, target_f, epochs=1, verbose=0)
                    reward = 0
            else:  # Random agent's turn
                row, col = get_random_move(game_state.board)
                valid_move = do_move(game_state.board, row, col, PLAYER_1) 
            next_state = get_state(game_state.board, game_state.player1StartFirst)
              # Reward for valid move, penalty for invalid
            next_turn(game_state)

            if game_state.winner != UNASSIGNED:
                done = True
                if game_state.winner == AI:
                    print("\033[92mAI Won this round")
                    reward += WIN_REWARD  # Reward for winning
                    wins += 1
                else:
                    print("\033[91mAI LOST this round")
                    reward += LOSE_REWARD  # Penalty for losing
                    losses += 1
            elif game_state.isDraw:
                print("\033[96mAI Drew this round")
                done = True
                reward += DRAW_REWARD  # Small reward for draw
                draws += 1
            else:
                # Reward shaping for creating a line of two
                for i in range(3):  # Check rows and columns
                    if (game_state.board[i][0] == game_state.board[i][1] == AI and game_state.board[i][2] == BOARD_EMPTY) or \
                    (game_state.board[i][0] == game_state.board[i][2] == AI and game_state.board[i][1] == BOARD_EMPTY) or \
                    (game_state.board[i][1] == game_state.board[i][2] == AI and game_state.board[i][0] == BOARD_EMPTY) or \
                    (game_state.board[0][i] == game_state.board[1][i] == AI and game_state.board[2][i] == BOARD_EMPTY) or \
                    (game_state.board[0][i] == game_state.board[2][i] == AI and game_state.board[1][i] == BOARD_EMPTY) or \
                    (game_state.board[1][i] == game_state.board[2][i] == AI and game_state.board[0][i] == BOARD_EMPTY):
                        reward += 20
            

            if game_state.turn == AI:  # Only update Q-learning agent
                # Q-learning update
                target = reward + gamma * np.max(model.predict(next_state)[0])
                target_f = model.predict(state)
                target_f[0][action] = target
                model.fit(state, target_f, epochs=1, verbose=0)

            state = next_state

        epsilon *= epsilon_decay
        if (episode + 1) % 1000 == 0:
            print(f"Episode: {episode + 1}, Epsilon: {epsilon:.4f}")
        if (episode + 1) % 5 == 0:
            # Append the current win/loss/draw counts to the history lists
            win_history.append(wins)
            loss_history.append(losses)
            draw_history.append(draws)
            wins = 0
            losses = 0
            draws = 0

    # Save the trained model weights
    model.export("weights")

    # Plotting the results
    plt.figure(figsize=(10, 5))
    plt.plot(win_history, label='Wins')
    plt.plot(loss_history, label='Losses')
    plt.plot(draw_history, label='Draws')
    plt.title('Q-learning Training Results')
    plt.xlabel('Episodes (in thousands)')
    plt.ylabel('Number of Wins/Losses/Draws')
    plt.legend()
    plt.show()
    plt.savefig('a.png')
    # Create best-fit line graph
    # plt.figure()
    # plt.plot(x, best_fit_line, linestyle='-', color='red', label='Best Fit Line')
    # plt.xlabel('X-axis')
    # plt.ylabel('Y-axis')
    # plt.title('Best Fit Line')
    # plt.legend()
    # plt.savefig('best_fit_line.png')

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