
#ifdef __cplusplus
extern "C" {
#endif

typedef struct robot_position {
	int id;
	int px;
	int py;
	int pa;
} gps_position_t;

int getGPS(gps_position_t * gps_data_ptr, int id);

#ifdef __cplusplus
}
#endif
