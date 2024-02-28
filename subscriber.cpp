/*
 * Copyright(c) 2006 to 2021 ZettaScale Technology and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "HelloWorldData.hpp"
#include "DdsMessages.hpp"

using namespace org::eclipse::cyclonedds;

int main() {
    try {
        std::cout << "=== [Subscriber] Create -arrosto- reader." << std::endl;

        /* First, a domain participant is needed.
         * Create one on the default domain. */
        dds::domain::DomainParticipant participant(0);

        dds::topic::qos::TopicQos tqos;
        tqos << dds::core::policy::History(dds::core::policy::HistoryKind::KEEP_LAST);
        tqos << dds::core::policy::Durability(dds::core::policy::DurabilityKind::TRANSIENT_LOCAL);
        tqos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);

        /* To subscribe to something, a topic is needed. */
        dds::topic::Topic<service::NotificationMsg> topic(participant, "HardwareControlLayer_Notifications", tqos);

        /* A reader also needs a subscriber. */
        dds::sub::Subscriber subscriber(participant);
        
        dds::sub::qos::DataReaderQos readerQos;

        readerQos = subscriber.default_datareader_qos();
        const auto topicQos = topic.qos();

        readerQos = topicQos;

        /* Now, the reader can be created to subscribe to a HelloWorld message. */
        dds::sub::DataReader<service::NotificationMsg> reader(subscriber, topic, readerQos, nullptr, dds::core::status::StatusMask::all());

        while (true) {
            auto samples = reader.take();
            for (auto sample : samples) {
                if (sample.info().valid()) {
                    auto data = sample.data();
                    std::cout << "Received message: ID= " << data.id() 
                    << "Received message: notification= " << data.notification() << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    } catch (const dds::core::Exception& e) {
        std::cerr << "=== [Subscriber] DDS exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "=== [Subscriber] C++ exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Subscriber] Done." << std::endl;

    return EXIT_SUCCESS;
}
