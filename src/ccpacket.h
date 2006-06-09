#ifndef __CCPACKET_H__
#define __CCPACKET_H__

enum command_t {
	CC_PAN_LEFT = 1 << 0,
	CC_PAN_RIGHT = 1 << 1,
	CC_TILT_UP = 1 << 2,
	CC_TILT_DOWN = 1 << 3,
	CC_ZOOM = 1 << 4,
	CC_RECALL = 1 << 5,
	CC_STORE = 1 << 6,
};

enum zoom_t {
	ZOOM_OUT = -1,
	ZOOM_NONE = 0,
	ZOOM_IN = 1,
};

enum focus_t {
	FOCUS_NEAR = -1,
	FOCUS_NONE = 0,
	FOCUS_FAR = 1,
};

enum iris_t {
	IRIS_CLOSE = -1,
	IRIS_NONE = 0,
	IRIS_OPEN = 1,
};

enum aux_t {
	AUX_NONE, AUX_1, AUX_2, AUX_3, AUX_4, AUX_5, AUX_6
};

/*
 * A camera control packet is a protocol-neutral representation of a single
 * message to a camera receiver driver.
 */
struct ccpacket {
	int	receiver;	/* receiver address: 1 to 255 */
	enum command_t	command;/* bitfield of commands */
	int		pan;	/* -1023 (fast left) to 1023 (fast right) */
	int		tilt;	/* -1023 (fast down) to 1023 (fast up) */
	enum zoom_t	zoom;	/* -1 (out), 0 (no change), or 1 (in) */
	enum focus_t	focus;	/* -1 (near), 0 (no change), or 1 (far) */
	enum iris_t	iris;	/* -1 (close), 0 (no change), or 1 (open) */
	enum aux_t	aux;	/* 0 (no function), or function 1-6 */
	int		preset;	/* preset number */
};

void ccpacket_init(struct ccpacket *p);
void ccpacket_debug(struct ccpacket *p);

#endif
