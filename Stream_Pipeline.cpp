#include <tbb/parallel_pipeline.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>
#include <iostream>
#include <vector>
#include <atomic>

// Mock structures for demonstration
struct Stream_Circle {
    int id;
};

struct Stream_Event {
};

struct Event_Queue {
};

struct Circle_Queue {
};

struct Centroiding {
    void run() {
        // std::cout << "Running Centroiding" << std::endl;
    }
};

struct Reconstruction_Result {
    // Placeholder structure
};

struct Vector3f {
    float x = 0, y = 0, z = 0;
};

class Stream_Pipeline {
public:
    Stream_Pipeline();

    void run_pipeline(); // Main function to execute the pipeline

private:
    void process_circle(const Stream_Circle &circle);

    void run_loc_iter(Vector3f &source, std::vector<Reconstruction_Result *> &circles);

    void run_nn(Vector3f &source, std::vector<Reconstruction_Result *> &circles);

    void run_loc_batch(std::vector<Reconstruction_Result> &_circles);

    void run_localization();

    void run_reconstruct(int threadId);

    void simulate_flight_client_from_memory();

    tbb::concurrent_queue<Stream_Circle> circle_queue; // Input queue
    std::atomic<int> processed_circles{0}; // Count of processed items

    Event_Queue event_queue;
};

Stream_Pipeline::Stream_Pipeline() = default;

void Stream_Pipeline::run_pipeline() {
    // Fill the queue with mock data
    simulate_flight_client_from_memory();

    // Number of concurrent tokens (parallelism level)
    const int max_concurrency = 10;
    const int n_trials = 10;

    // in serial
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, n_trials),
        [&](const tbb::blocked_range<size_t>& range) {
            for (size_t i = range.begin(); i < range.end(); ++i) {
                Centroiding centroiding;
                centroiding.run();

                // Input stage: Generate or retrieve Stream_Circle objects
                auto input_stage = tbb::make_filter<void, Stream_Circle>(
                    tbb::filter_mode::parallel, [&](tbb::flow_control &fc) -> Stream_Circle {
                        Stream_Circle circle;
                        if (!circle_queue.try_pop(circle)) {
                            fc.stop(); // No more data; stop the pipeline
                            return {};
                        }
                        circle.id += 1;
                        std::cout << "Stage 1: Processed circle with ID: " << circle.id << std::endl;
                        return circle; // Pass circle to the next stage
                    });

                // Processing stage: Perform computations in parallel
                auto processing_stage = tbb::make_filter<Stream_Circle, Stream_Circle>(
                    tbb::filter_mode::parallel, [&](Stream_Circle circle) -> Stream_Circle {
                        process_circle(circle); // Perform localization and NN refinement
                        circle.id *= 2;
                        std::cout << "Stage 2: Processed circle with ID: " << circle.id << std::endl;
                        return circle;
                    });

                // Output stage: Handle the processed results
                auto output_stage = tbb::make_filter<Stream_Circle, void>(
                    tbb::filter_mode::serial_in_order, [&](Stream_Circle circle) {
                        processed_circles++;
                    });

                // Construct and run the pipeline
                tbb::parallel_pipeline(max_concurrency, input_stage & processing_stage & output_stage);
            }
        }
    );

    std::cout << "Total circles processed: " << processed_circles.load() << std::endl;
}

void Stream_Pipeline::process_circle(const Stream_Circle &circle) {
    // Example processing logic
    Vector3f source;
    std::vector<Reconstruction_Result *> circles;

    // Run localization and NN refinement
    run_loc_iter(source, circles);
    run_nn(source, circles);
}

void Stream_Pipeline::run_loc_iter(Vector3f &source, std::vector<Reconstruction_Result *> &circles) {
    // std::cout << "Running localization iteration..." << std::endl;
}

void Stream_Pipeline::run_nn(Vector3f &source, std::vector<Reconstruction_Result *> &circles) {
    // std::cout << "Running neural network refinement..." << std::endl;
}

void Stream_Pipeline::run_loc_batch(std::vector<Reconstruction_Result> &_circles) {
    // std::cout << "Running localization batch..." << std::endl;
}

void Stream_Pipeline::run_localization() {
    // std::cout << "Running localization iteration..." << std::endl;
}

void Stream_Pipeline::run_reconstruct(int threadId) {
    // std::cout << "Running reconstruction iteration..." << std::endl;
}

void Stream_Pipeline::simulate_flight_client_from_memory() {
    for (int i = 0; i < 100; ++i) {
        circle_queue.push({i});
    }
}

int main() {
    Stream_Pipeline pipeline;
    pipeline.run_pipeline(); // 执行流水线
    return 0;
}