CREATE KEYSPACE IF NOT EXISTS pulse
  WITH replication = {'class': 'SimpleStrategy', 'replication_factor': 1};