/*
 * Copyright(c) 2006 to 2020 ZettaScale Technology and others
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
        std::cout << "=== [Publisher] Create writer." << std::endl;

        /* First, a domain participant is needed.
         * Create one on the default domain. */
        dds::domain::DomainParticipant participant(0);

        /* To publish something, a topic is needed. */
        dds::topic::qos::TopicQos tqos = participant.default_topic_qos();
        tqos << dds::core::policy::Reliability::Reliable()
            << dds::core::policy::Durability::TransientLocal()
            << dds::core::policy::History::KeepLast(3);

        /* To publish something, a topic is needed. */
        dds::topic::Topic<service::NotificationMsg> topic(participant, "HardwareControlLayer_Notifications", tqos);

        /* A writer also needs a publisher. */
        dds::pub::Publisher publisher(participant);

        /* Now, the writer can be created to publish a HelloWorld message. */
        dds::pub::qos::DataWriterQos writer_qos(tqos);
        dds::pub::DataWriter<service::NotificationMsg> writer(publisher, topic, writer_qos);
       
        std::cout << "=== [Publisher] Write sample." << std::endl;
        for (uint16_t i = 0; i < 100; ++i) {
            service::NotificationMsg message;
            message = service::NotificationMsg{i, static_cast<int16_t>(i), ::service::Value()};
            writer.write(message);
            std::cout << "Sending message: " << message.id() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    catch (const dds::core::Exception& e) {
        std::cerr << "=== [Publisher] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Publisher] Done." << std::endl;

    return EXIT_SUCCESS;
}
