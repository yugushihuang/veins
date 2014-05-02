//
// Copyright (C) 2006-2012 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef VEINS_WORLD_TRACI_TRACISCENARIOMANAGER_H
#define VEINS_WORLD_TRACI_TRACISCENARIOMANAGER_H

#include <map>
#include <list>
#include <queue>

#include <omnetpp.h>

#include "Coord.h"
#include "BaseWorldUtility.h"
#include "BaseConnectionManager.h"
#include "FindModule.h"
#include "modules/obstacle/ObstacleControl.h"
#include "modules/mobility/traci/TraCIBuffer.h"
#include "modules/mobility/traci/TraCIColor.h"
#include "modules/mobility/traci/TraCIConnection.h"
#include "modules/mobility/traci/TraCICoord.h"

/**
 * @brief
 * Creates and moves nodes controlled by a TraCI server.
 *
 * If the server is a SUMO road traffic simulation, you can use the
 * TraCIScenarioManagerLaunchd module and sumo-launchd.py script instead.
 *
 * All nodes created thus must have a TraCIMobility submodule.
 *
 * See the Veins website <a href="http://veins.car2x.org/"> for a tutorial, documentation, and publications </a>.
 *
 * @author Christoph Sommer, David Eckhoff, Falko Dressler, Zheng Yao, Tobias Mayer, Alvaro Torres Cortes, Luca Bedogni
 *
 * @see TraCIMobility
 * @see TraCIScenarioManagerLaunchd
 *
 */
namespace Veins {
class TraCIScenarioManager : public cSimpleModule
{
	public:

		enum VehicleSignal {
			VEH_SIGNAL_UNDEF = -1,
			VEH_SIGNAL_NONE = 0,
			VEH_SIGNAL_BLINKER_RIGHT = 1,
			VEH_SIGNAL_BLINKER_LEFT = 2,
			VEH_SIGNAL_BLINKER_EMERGENCY = 4,
			VEH_SIGNAL_BRAKELIGHT = 8,
			VEH_SIGNAL_FRONTLIGHT = 16,
			VEH_SIGNAL_FOGLIGHT = 32,
			VEH_SIGNAL_HIGHBEAM = 64,
			VEH_SIGNAL_BACKDRIVE = 128,
			VEH_SIGNAL_WIPER = 256,
			VEH_SIGNAL_DOOR_OPEN_LEFT = 512,
			VEH_SIGNAL_DOOR_OPEN_RIGHT = 1024,
			VEH_SIGNAL_EMERGENCY_BLUE = 2048,
			VEH_SIGNAL_EMERGENCY_RED = 4096,
			VEH_SIGNAL_EMERGENCY_YELLOW = 8192
		};

		TraCIScenarioManager();
		enum DepartDefs {
			DEPART_NOW = 2,
			DEPART_LANE_BEST_FREE = 5,
			DEPART_POS_BASE = 4,
			DEPART_SPEED_MAX = 3
		};

		~TraCIScenarioManager();
		virtual int numInitStages() const { return std::max(cSimpleModule::numInitStages(), 2); }
		virtual void initialize(int stage);
		virtual void finish();
		virtual void handleMessage(cMessage *msg);
		virtual void handleSelfMsg(cMessage *msg);

		bool isConnected() const { return (connection); }

		std::pair<uint32_t, std::string> commandGetVersion();
		void commandSetSpeedMode(std::string nodeId, int32_t bitset);
		void commandSetSpeed(std::string nodeId, double speed);
		void commandSetColor(std::string nodeId, TraCIColor& color);
		void commandSlowDown(std::string nodeId, double speed, int time);
		void commandNewRoute(std::string nodeId, std::string roadId);
		void commandSetVehicleParking(std::string nodeId);
		double commandGetEdgeCurrentTravelTime(std::string edgeId) ;
		double commandGetEdgeMeanSpeed(std::string edgeId) ;
		std::string commandGetEdgeId(std::string nodeId);
		std::string commandGetCurrentEdgeOnRoute(std::string nodeId);
		std::string commandGetLaneId(std::string nodeId);
		double commandGetLanePosition(std::string nodeId);
		std::list<std::string> commandGetPlannedEdgeIds(std::string nodeId);
		std::string commandGetRouteId(std::string nodeId);
		std::list<std::string> commandGetRouteEdgeIds(std::string routeId);
		void commandChangeRoute(std::string nodeId, std::string roadId, double travelTime);
		double commandDistanceRequest(Coord position1, Coord position2, bool returnDrivingDistance);
		void commandStopNode(std::string nodeId, std::string roadId, double pos, uint8_t laneid, double radius, double waittime);
		void commandSetTrafficLightProgram(std::string trafficLightId, std::string program);
		void commandSetTrafficLightPhaseIndex(std::string trafficLightId, int32_t index);
		std::list<std::string> commandGetPolygonIds();
		std::string commandGetPolygonTypeId(std::string polyId);
		std::list<Coord> commandGetPolygonShape(std::string polyId);
		void commandSetPolygonShape(std::string polyId, std::list<Coord> points);
		void commandAddPolygon(std::string polyId, std::string polyType, const TraCIColor& color, bool filled, int32_t layer, std::list<Coord> points);
		void commandRemovePolygon(std::string polyId, int32_t layer);
		void commandAddPoi(std::string poiId, std::string poiType, const TraCIColor& color, int32_t layer, Coord pos);
		void commandRemovePoi(std::string poiId, int32_t layer);
		std::list<std::string> commandGetLaneIds();
		std::list<Coord> commandGetLaneShape(std::string laneId);
		std::string commandGetLaneEdgeId(std::string laneId);
		double commandGetLaneLength(std::string laneId);
		double commandGetLaneMaxSpeed(std::string laneId);
		double commandGetLaneMeanSpeed(std::string laneId);
		int32_t commandGetLaneIndex(std::string nodeId);
		std::list<std::string> commandGetJunctionIds();
		Coord commandGetJunctionPosition(std::string junctionId);
		bool commandAddVehicle(std::string vehicleId, std::string vehicleTypeId, std::string routeId, simtime_t emitTime_st = -DEPART_NOW, double emitPosition = -DEPART_POS_BASE, double emitSpeed = -DEPART_SPEED_MAX, int8_t emitLane = -DEPART_LANE_BEST_FREE);
		std::string commandGetVehicleTypeId(std::string nodeId);
		std::list<std::string> commandGetVehicleTypeIds();
		std::list<std::string> commandGetRouteIds();
		bool commandChangeVehicleRoute(std::string nodeId, std::list<std::string> edges);
		std::pair<double, double> commandPositionConversionLonLat(const Coord&);
		bool getAutoShutdownTriggered() {
			return autoShutdownTriggered;
		}

		const std::map<std::string, cModule*>& getManagedHosts() {
			return hosts;
		}

		/**
		 * convert TraCI coordinates to OMNeT++ coordinates
		 */
		Coord traci2omnet(TraCICoord coord) const;

		/**
		 * convert OMNeT++ coordinates to TraCI coordinates
		 */
		TraCICoord omnet2traci(Coord coord) const;

	protected:
		bool debug; /**< whether to emit debug messages */
		simtime_t connectAt; /**< when to connect to TraCI server (must be the initial timestep of the server) */
		simtime_t firstStepAt; /**< when to start synchronizing with the TraCI server (-1: immediately after connecting) */
		simtime_t updateInterval; /**< time interval of hosts' position updates */
		std::string moduleType; /**< module type to be used in the simulation for each managed vehicle */
		std::string moduleName; /**< module name to be used in the simulation for each managed vehicle */
		std::string moduleDisplayString; /**< module displayString to be used in the simulation for each managed vehicle */
		std::string host;
		int port;

		uint32_t vehicleNameCounter;
		cMessage* myAddVehicleTimer;
		std::vector<std::string> vehicleTypeIds;
		std::map<int, std::queue<std::string> > vehicleInsertQueue;
		std::set<std::string> queuedVehicles;
		std::vector<std::string> routeIds;
		int vehicleRngIndex;
		int numVehicles;

		cRNG* mobRng;

		bool autoShutdown; /**< Shutdown module as soon as no more vehicles are in the simulation */
		int margin;
		double penetrationRate;
		std::list<std::string> roiRoads; /**< which roads (e.g. "hwy1 hwy2") are considered to consitute the region of interest, if not empty */
		std::list<std::pair<TraCICoord, TraCICoord> > roiRects; /**< which rectangles (e.g. "0,0-10,10 20,20-30,30) are considered to consitute the region of interest, if not empty */

		TraCIConnection* connection;
		TraCICoord netbounds1; /* network boundaries as reported by TraCI (x1, y1) */
		TraCICoord netbounds2; /* network boundaries as reported by TraCI (x2, y2) */

		size_t nextNodeVectorIndex; /**< next OMNeT++ module vector index to use */
		std::map<std::string, cModule*> hosts; /**< vector of all hosts managed by us */
		std::set<std::string> unEquippedHosts;
		std::set<std::string> subscribedVehicles; /**< all vehicles we have already subscribed to */
		uint32_t activeVehicleCount; /**< number of vehicles, be it parking or driving **/
		uint32_t parkingVehicleCount; /**< number of parking vehicles, derived from parking start/end events */
		uint32_t drivingVehicleCount; /**< number of driving, as reported by sumo */
		bool autoShutdownTriggered;
		cMessage* connectAndStartTrigger; /**< self-message scheduled for when to connect to TraCI server and start running */
		cMessage* executeOneTimestepTrigger; /**< self-message scheduled for when to next call executeOneTimestep */

		BaseWorldUtility* world;
		BaseConnectionManager* cc;

		uint32_t getCurrentTimeMs(); /**< get current simulation time (in ms) */

		void executeOneTimestep(); /**< read and execute all commands for the next timestep */

		virtual void init_traci();

		void addModule(std::string nodeId, std::string type, std::string name, std::string displayString, const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
		cModule* getManagedModule(std::string nodeId); /**< returns a pointer to the managed module named moduleName, or 0 if no module can be found */
		void deleteModule(std::string nodeId);

		bool isModuleUnequipped(std::string nodeId); /**< returns true if this vehicle is Unequipped */

		/**
		 * returns whether a given position lies within the simulation's region of interest.
		 * Modules are destroyed and re-created as managed vehicles leave and re-enter the ROI
		 */
		bool isInRegionOfInterest(const TraCICoord& position, std::string road_id, double speed, double angle);

		/**
		 * commonly employed technique to get string values via TraCI
		 */
		std::string genericGetString(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
		Coord genericGetCoord(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
		double genericGetDouble(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
		int32_t genericGetInt(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
		std::list<std::string> genericGetStringList(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
		std::list<Coord> genericGetCoordList(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);

		/**
		 * convert TraCI angle to OMNeT++ angle (in rad)
		 */
		double traci2omnetAngle(double angle) const;

		/**
		 * convert OMNeT++ angle (in rad) to TraCI angle
		 */
		double omnet2traciAngle(double angle) const;

		/**
		 * Convert TraCI coord to a pair of longitude and latitude
		 */
		std::pair<double, double> commandPositionConversionLonLat(const TraCICoord&);

		/**
		 * adds a new vehicle to the queue which are tried to be inserted at the next SUMO time step;
		 */
		void insertNewVehicle();

		/**
		 * tries to add all vehicles in the vehicle queue to SUMO;
		 */
		void insertVehicles();

		void subscribeToVehicleVariables(std::string vehicleId);
		void unsubscribeFromVehicleVariables(std::string vehicleId);
		void processSimSubscription(std::string objectId, TraCIBuffer& buf);
		void processVehicleSubscription(std::string objectId, TraCIBuffer& buf);
		void processSubcriptionResult(TraCIBuffer& buf);

};
}

namespace Veins {
class TraCIScenarioManagerAccess
{
	public:
		TraCIScenarioManager* get() {
			return FindModule<TraCIScenarioManager*>::findGlobalModule();
		};
};
}

#endif
