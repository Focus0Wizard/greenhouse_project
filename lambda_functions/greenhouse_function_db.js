import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

export const handler = async (event, context) => {
    const command = new PutCommand({
        TableName: "greenhouse_data",
        Item: {
            timestamp: event.timestamp,
            thing_name: event.thing_name,
            sn: event.sn,
            tempState: event.tempState,
            luxState: event.luxState
        },
    });

    const response = await docClient.send(command);
    console.log(response);
};