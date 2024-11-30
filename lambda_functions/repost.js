import { IoTDataPlaneClient, UpdateThingShadowCommand, GetThingShadowCommand } from '@aws-sdk/client-iot-data-plane';
import { DynamoDBClient } from '@aws-sdk/client-dynamodb';
import { DynamoDBDocumentClient, ScanCommand } from '@aws-sdk/lib-dynamodb';

const iotClient = new IoTDataPlaneClient({ region: 'us-east-1' });
const dynamoDBClient = new DynamoDBClient({ region: 'us-east-1' });
const docClient = DynamoDBDocumentClient.from(dynamoDBClient);

async function getThingNameFromDynamoDB() {
    const params = {
        TableName: 'greenhouse_data',
        Limit: 1,
    };

    try {
        console.log("Obtaining 'thingName' from DynamoDB...");
        const command = new ScanCommand(params);
        const response = await docClient.send(command);

        if (response.Items && response.Items.length > 0) {
            console.log("'thingName' found at DynamoDB:", response.Items[0].thing_name);
            return response.Items[0].thing_name;
        } else {
            throw new Error("No 'thingName' found in DynamoDB.");
        }
    } catch (error) {
        console.error('Error getting thingName from DynamoDB:', error);
        throw error;
    }
}

async function getThingShadowState(thingName) {
    const getParams = {
        thingName: thingName,
    };

    try {
        console.log(`Getting the Shadow status for the device: ${thingName}`);
        const command = new GetThingShadowCommand(getParams);
        const response = await iotClient.send(command);

        if (response.payload) {
            const payload = JSON.parse(new TextDecoder().decode(response.payload));
            console.log('Current state of the Shadow:', payload.state.reported);
            return payload.state.reported;
        } else {
            throw new Error('Shadow status could not be obtained.');
        }
    } catch (error) {
        console.error('Error getting shadow status:', error);
        throw error;
    }
}

async function updateThingShadow(thingName, irrigation_state, vent_state) {
    const shadowPayload = {
        state: {
            reported: {
                irrigation_state: irrigation_state,
                vent_state: vent_state,
            },
        },
    };

    const updateParams = {
        thingName: thingName,
        payload: JSON.stringify(shadowPayload),
    };

    try {
        console.log(`Updating Shadow for the device: ${thingName}`);
        const command = new UpdateThingShadowCommand(updateParams);
        const response = await iotClient.send(command);

        if (response.payload) {
            console.log('Shadow successfully updated:', JSON.parse(new TextDecoder().decode(response.payload)));
            return JSON.parse(new TextDecoder().decode(response.payload));
        } else {
            throw new Error('Error while updating the shadow.');
        }
    } catch (error) {
        console.error('Error while updating the shadow:', error);
        throw error;
    }
}

export const handler = async (event) => {
    try {
        console.log('Event received:', JSON.stringify(event, null, 2));

        let thingName = event['thing_name'];
        if (!thingName) {
            console.log("No 'thingName' found in the event. Trying to get from DynamoDB...");
            thingName = await getThingNameFromDynamoDB();
        }

        let tempState = null;
        let luxState = null;

        if (event.state?.reported) {
            tempState = event.state.reported.tempState;
            luxState = event.state.reported.luxState;
        } else if (event.tempState !== undefined && event.luxState !== undefined) {
            tempState = event.tempState;
            luxState = event.luxState;
        }

        if (tempState === null || luxState === null) {
            console.error("No 'tempState' or 'luxState' found in the event:", JSON.stringify(event));
            throw new Error("Missing values of 'tempState' or 'luxState' in the event.");
        }

        console.log(`Extracted values: tempState=${tempState}, luxState=${luxState}`);

        const irrigation_state = tempState === 1 ? 1 : 0;
        const vent_state = luxState === 1 ? 1 : 0;

        console.log(`Calculated states: irrigation_state=${irrigation_state}, vent_state=${vent_state}`);

        const currentShadowState = await getThingShadowState(thingName);

        if (
            currentShadowState.irrigation_state === irrigation_state &&
            currentShadowState.vent_state === vent_state
        ) {
            console.log("You do not need to update the Shadow. The states are the same.");
            return {
                statusCode: 200,
                body: JSON.stringify({ message: "The Shadow is already updated." }),
            };
        }

        const updatedShadow = await updateThingShadow(thingName, irrigation_state, vent_state);

        return {
            statusCode: 200,
            body: JSON.stringify({ message: "Shadow updated successfully.", updatedShadow }),
        };
    } catch (error) {
        console.error('Lambda function error:', error);
        return {
            statusCode: 500,
            body: JSON.stringify({ message: "Error processing the event.", error: error.message }),
        };
    }
};