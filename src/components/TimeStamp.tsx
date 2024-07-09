import React from 'react';
import { formatDistanceToNow, parseISO } from 'date-fns';

interface TimestampConverterProps {
  timestamp: string; // ISO 8601 format
}

const TimestampConverter: React.FC<TimestampConverterProps> = ({ timestamp }) => {
  // Parse the ISO timestamp string into a Date object
  const date = parseISO(timestamp);

  // Calculate the distance from now
  const timeAgo = formatDistanceToNow(date, { addSuffix: true });
  return <>{timeAgo}</>;
};

export default TimestampConverter;