#include <stdio.h>
#include <stdlib.h>
#include <include/definitions.h>
#include <include/util.h>
#include <include/minimax.h>
#include <include/game.h>
#include <include/deep_q.h>
#include <sys/stat.h>

// Load the saved model
TF_Graph *graph = NULL;
TF_Session *session = NULL;
TF_Status *status = NULL;

static void free_buffer(void *data, size_t length)
{
    free(data);
}

/// @brief checks the output of all the graph's operations, given a dummy input
/// @param graph 
void check_output_tensors(TF_Graph *graph)
{
    size_t pos = 0;
    TF_Operation *oper;

    printf("Checking output tensors:\n");
    while ((oper = TF_GraphNextOperation(graph, &pos)) != NULL)
    {
        println("Running...");
        int num_outputs = TF_OperationNumOutputs(oper);
        for (int i = 0; i < num_outputs; ++i)
        {
            int64_t output_dims[1] = {9};
            size_t output_tensor_size = 9 * sizeof(float);
            TF_Output output = {oper, i};
            TF_Tensor *output_tensor = TF_AllocateTensor(TF_FLOAT, output_dims, 1, output_tensor_size);

            // Create a dummy input tensor (replace with your actual input if needed)
            int64_t input_dim[1] = {9};

            TF_Tensor *input_tensor = TF_AllocateTensor(TF_FLOAT, input_dim, 1, 9 * sizeof(float));
            float *data = (float *)TF_TensorData(input_tensor);
            for (int j = 0; j < 9; ++j)
            {
                data[j] = 0.0f;
            }

            TF_Output input_op = {TF_GraphOperationByName(graph, "serving_default_keras_tensor"), 0};
            TF_Output inputs[] = {input_op};
            TF_Tensor *input_values[] = {input_tensor};
            TF_Output outputs[] = {output};
            TF_Tensor *output_values[] = {output_tensor};

            TF_SessionRun(session, NULL, inputs, input_values, 1, outputs, output_values, 1, NULL, 0, NULL, status);

            if (TF_GetCode(status) == TF_OK && output_tensor != NULL)
            {
                printf("  - %s:%d is VALID\n", TF_OperationName(oper), i);
                // You can further inspect the output_tensor here if needed
            }
            else if (TF_GetCode(status) == TF_OK)
            {
                printf("  - %s:%d is NULL\n", TF_OperationName(oper), i);
            }

            // Clean up
            TF_DeleteTensor(input_tensor);
            if (output_tensor != NULL)
            {
                TF_DeleteTensor(output_tensor);
            }
        }
    }
}

/// @brief lists all possible operations with the current tensorflow graph
/// @param graph 
void list_operations(TF_Graph *graph)
{
    if (graph == NULL)
    {
        fprintf(stderr, "ERROR: Graph is NULL\n");
        return;
    }

    size_t pos = 0;
    TF_Operation *oper;

    printf("Operations in the graph:\n");
    while ((oper = TF_GraphNextOperation(graph, &pos)) != NULL)
    {
        // Add a check to make sure 'oper' is not NULL before accessing its name
        if (oper == NULL)
        {
            fprintf(stderr, "ERROR: Encountered a NULL operation\n");
            break;
        }
        printf("  - %s\n", TF_OperationName(oper));
    }
}

// Initialize TensorFlow and load the model
void init_tensorflow(const char *model_path)
{
    status = TF_NewStatus();
    graph = TF_NewGraph();

    TF_SessionOptions *sess_opts = TF_NewSessionOptions();
    TF_Buffer *run_options = TF_NewBufferFromString("", 0);
    const char *tags = "serve"; // Or other relevant tag
    int ntags = 1;

    session = TF_LoadSessionFromSavedModel(sess_opts, run_options, model_path, &tags, ntags, graph, NULL, status);

    if (TF_GetCode(status) != TF_OK)
    {
        fprintf(stderr, "ERROR: Unable to load SavedModel: %s\n", TF_Message(status));
        exit(1);
    }
    // graph = session->graph;

    TF_DeleteBuffer(run_options);
    TF_DeleteSessionOptions(sess_opts);

    // Now, 'graph' should be populated, you can list operations here
    list_operations(graph);
    check_output_tensors(graph);
}

// Function to perform inference
Pair findBestDLMove(int board[3][3], PlayerType currentPlayer, bool playerStartFirst)
{
    // Convert board state to a tensor (with float values)
    int64_t input_dim[1] = {9};
    TF_Tensor *input_tensor = TF_AllocateTensor(TF_FLOAT, input_dim, 1, 9 * sizeof(float)); // Use TF_FLOAT
    float *data = (float *)TF_TensorData(input_tensor);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if(playerStartFirst){
                if(board[i][j] == 1){
                    board[i][j] == 2;
                }else if(board[i][j] == 2){
                    board[i][j] == 1;
                }
            }
            data[i * 3 + j] = (float)board[i][j]; // Cast int to float
        }
    }

    TF_Output input_op = {TF_GraphOperationByName(graph, "serving_default_keras_tensor"), 0};
    if (input_op.oper == NULL)
    {
        println("ERROR: Input operation not found!");
        exit(1);
    }

    TF_Output output_op = {TF_GraphOperationByName(graph, "StatefulPartitionedCall_1"), 0}; // Use the correct output op name
    if (output_op.oper == NULL)
    {
        println("ERROR: Output operation not found!");
        exit(1);
    }

    // Create input and output values
    // Allocate memory for the output tensor
    int64_t output_dims[1] = {9};
    size_t output_tensor_size = 9 * sizeof(float);
    TF_Tensor *output_tensor = TF_AllocateTensor(TF_FLOAT, output_dims, 1, output_tensor_size);
    TF_Output outputs[] = {output_op};

    TF_Tensor *output_values[] = {output_tensor};
    TF_Output inputs[] = {input_op};
    TF_Tensor *input_values[] = {input_tensor};

    // Run the session
    TF_SessionRun(session, NULL, inputs, input_values, 1, outputs, output_values, 1, NULL, 0, NULL, status);
    char *debug_string = TF_Message(status);

    if (TF_GetCode(status) != TF_OK)
    {
        fprintf(stderr, "ERROR: Session run failed: %s\n", debug_string);
        exit(1);
    }

    // debug the output of tensorflow
    fprintf(stderr, "Debug string: %s\n", debug_string);

    // check if output_tensor is valid before accessing its data
    if (output_tensor == NULL)
    {
        fprintf(stderr, "ERROR: Output tensor is NULL\n");
        exit(1);
    }

    if (TF_TensorType(output_tensor) != TF_FLOAT)
    {
        fprintf(stderr, "ERROR: Output tensor is not of type TF_FLOAT\n");
        exit(1);
    }

    // get the predicted Q-values
    float *q_values = (float *)TF_TensorData(output_tensor);

    // Determine the best move based on Q-values
    Pair bestMove;


    int best_move = -1;
    float best_q_value = -1000.0f; // initialize with a very low value, so we can compare it against the tensor and pick the best
    for (int i = 0; i < 9; ++i)
    {
        if (q_values[i] > best_q_value && data[i] == 0)
        {
            // ensure that the move is valid
            if(board[i / 3][i % 3] == 0){
                best_q_value = q_values[i];
                best_move = i;
            }
        }
    }
    
    // Convert best_move to row and col
    bestMove.a = best_move / 3;
    bestMove.b = best_move % 3;

    

    // Clean up
    TF_DeleteTensor(input_tensor);
    TF_DeleteTensor(output_tensor);
    return bestMove;
}

// Cleanup TensorFlow resources
void cleanup_tensorflow()
{
    TF_DeleteSession(session, status);
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);
}