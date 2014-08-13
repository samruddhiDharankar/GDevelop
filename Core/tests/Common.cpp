/*
 * Game Develop Core
 * Copyright 2008-2014 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the GNU Lesser General Public License.
 */
/**
 * @file Tests covering common features of Game Develop Core.
 */
#include "catch.hpp"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/SystemStats.h"
#include "GDCore/PlatformDefinition/Project.h"
#include "GDCore/PlatformDefinition/Layout.h"
#include "GDCore/Events/EventsList.h"
#include "GDCore/Events/Event.h"
#include "GDCore/Events/Builtin/StandardEvent.h"
#include "GDCore/Events/Serialization.h"
#include "GDCore/Serialization/Serializer.h"

TEST_CASE( "Common tools", "[common]" ) {
    REQUIRE( gd::ToInt("1") == 1 );
    REQUIRE( gd::ToString(2) == "2" );
}

TEST_CASE( "Project", "[common]" ) {
	SECTION("Basics") {
		gd::Project project;
		project.SetName("myname");
	    REQUIRE( project.GetName() == "myname" );
	}
}

TEST_CASE( "EventsList", "[common][events]" ) {

	SECTION("Basics") {
		gd::EventsList list;
		gd::EmptyEvent event1;
		boost::shared_ptr<gd::BaseEvent> event2(new gd::EmptyEvent);
		list.InsertEvent(event1);
		list.InsertEvent(event2);
	    REQUIRE( &list.GetEvent(0) != &event1 ); //First event inserted by copy
	    REQUIRE( &list.GetEvent(1) == event2.get() ); //Second event not copied	
	}

	SECTION("Subevents") {
		gd::EventsList list;
		gd::StandardEvent stdEvent;
		list.InsertEvent(stdEvent, 0);

		//Create a lots of nested events
		gd::BaseEvent * lastEvent = &list.GetEvent(0);
		gd::BaseEvent * oneOfTheSubEvent = NULL;
	    for(unsigned int i=0;i<100;++i) {
			gd::StandardEvent subEvent;
	    	lastEvent = &lastEvent->GetSubEvents().InsertEvent(subEvent);
	    	if ( i == 60 ) oneOfTheSubEvent = lastEvent;
	    }

	    //Check if Contains method can find the specified sub event.
	    REQUIRE(list.Contains(*oneOfTheSubEvent) == true);
	    REQUIRE(list.Contains(*oneOfTheSubEvent, false) == false);
	}

	SECTION("Memory consumption") {
	    size_t startMemory = gd::SystemStats::GetUsedVirtualMemory();

		gd::EventsList list;
		gd::StandardEvent stdEvent;
		list.InsertEvent(stdEvent, 0);

		//Create a lots of nested events
		gd::BaseEvent * lastEvent = &list.GetEvent(0);
	    for(unsigned int i=0;i<2000;++i) {
			gd::StandardEvent subEvent;
	    	lastEvent = &lastEvent->GetSubEvents().InsertEvent(subEvent);
	    }

	    //Copy the result
	    gd::EventsList copiedList = list;

	    size_t endMemory = gd::SystemStats::GetUsedVirtualMemory();
	    INFO("Memory used: " << endMemory-startMemory << "KB");
	    REQUIRE(1500 >= endMemory-startMemory);
	}

}