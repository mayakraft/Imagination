
float hueToRgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1./6) return p + (q - p) * 6 * t;
	if (t < 1./2) return q;
	if (t < 2./3) return p + (q - p) * (2./3 - t) * 6;
	return p;
}

// h, s, l between 0...1. r, g, b between 0...255
void hslToRgb(
	float h, float s, float l,
	unsigned char *red, unsigned char *green, unsigned char *blue) {
  if (0 == s) {
		*red = l * 255;
		*green = l * 255;
		*blue = l * 255;
		return;
  }
	float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	float p = 2 * l - q;
	*red = (unsigned char)(hueToRgb(p, q, h + 1./3) * 255);
	*green = (unsigned char)(hueToRgb(p, q, h) * 255);
	*blue = (unsigned char)(hueToRgb(p, q, h - 1./3) * 255);
}
