#include "TrafficLight.h"
#include <chrono>
#include <future>
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // FP.5a : The method receive should use std::unique_lock<std::mutex> and
  // _condition.wait() to wait for and receive new messages and pull them from
  // the queue using move semantics. The received object should then be
  // returned by the receive function.
  std::unique_lock<std::mutex> uLock(_mutex);
  _condVar.wait(uLock, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // FP.4a : The method send should use the mechanisms
  // std::lock_guard<std::mutex> as well as _condition.notify_one() to add a
  // new message to the queue and afterwards send a notification.
  std::lock_guard<std::mutex> uLock(_mutex);
  _queue.clear();
  _queue.emplace_back(std::move(msg));
  _condVar.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }
TrafficLight::~TrafficLight(){};

void TrafficLight::waitForGreen() {
  // FP.5b : add the implementation of the method waitForGreen, in which an
  // infinite while-loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  TrafficLightPhase phase;
  while (true) {
    phase = _messageQueue.receive();
    if (phase == TrafficLightPhase::green) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // FP.2b : Finally, the private method „cycleThroughPhases“ should be
  // started in a thread when the public method „simulate“ is called. To do
  // this, use the thread queue in the base class.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  // FP.2a : Implement the function with an infinite loop that measures the
  // time between two loop cycles
  // and toggles the current phase of the traffic light between red and green
  // and sends an update method
  // to the message queue using move semantics. The cycle duration should be a
  // random value between 4 and 6 seconds.
  // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms
  // between two cycles.

  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> end;
  std::chrono::duration<double> elapsed;

  std::random_device r;
  std::default_random_engine random_engine(r());
  std::uniform_int_distribution<int> uniform_dist(4, 6);
  int traffic_light_duration;

  while (true) {
    // start = std::chrono::system_clock::now();
    traffic_light_duration = uniform_dist(random_engine);
    std::this_thread::sleep_for(std::chrono::seconds(traffic_light_duration));
    togglePhase();
    _messageQueue.send(getCurrentPhase());
    // end = std::chrono::system_clock::now();
    // elapsed = end - start;
    //  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void TrafficLight::togglePhase() {
  if (_currentPhase == TrafficLightPhase::red) {
    _currentPhase = TrafficLightPhase::green;
  } else {
    _currentPhase = TrafficLightPhase::red;
  }
}
