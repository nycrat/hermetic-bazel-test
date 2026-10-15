#include <iostream>
#include <memory>

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

namespace {

// g3log's built-in default logger writes to a file. This sink forwards messages
// to stdout so the demo is visible when run with `bazel run //main:logging-demo`.
class StdoutSink {
 public:
  void receiveLogMessage(g3::LogMessageMover message) {
    std::cout << message.get().toString() << std::flush;
  }
};

}  // namespace

int main() {
  auto worker = g3::LogWorker::createLogWorker();
  worker->addSink(std::make_unique<StdoutSink>(), &StdoutSink::receiveLogMessage);
  g3::initializeLogging(worker.get());

  LOG(INFO) << "Hello from g3log!";
  LOG(WARNING) << "g3log is wired in as an external Bazel dependency.";

  // The LogWorker destructor shuts logging down and flushes pending messages.
  return 0;
}
