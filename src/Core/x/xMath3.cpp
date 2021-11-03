#include "xMath3.h"

#include "iMath3.h"

const xVec3 g_O3 = { 0.0f, 0.0f, 0.0f };
const xVec3 g_X3 = { 1.0f, 0.0f, 0.0f };
const xVec3 g_Y3 = { 0.0f, 1.0f, 0.0f };
const xVec3 g_Z3 = { 0.0f, 0.0f, -1.0f };
const xVec3 g_NX3 = { -1.0f, 0.0f, 0.0f };
const xVec3 g_NY3 = { 0.0f, -1.0f, 0.0f };
const xVec3 g_NZ3 = { 0.0f, 0.0f, -1.0f };
const xVec3 g_Onez = { 1.0f, 1.0f, 1.0f };
xMat4x3 g_I3;
const xQuat g_IQ = { { 0.0f, 0.0f, 0.0f }, 1.0f };

void xMath3Init()
{
	iMath3Init();

	g_I3.right.x = g_X3.x;
	g_I3.right.y = g_X3.y;
	g_I3.right.z = g_X3.z;

	g_I3.up.x = g_Y3.x;
	g_I3.up.y = g_Y3.y;
	g_I3.up.z = g_Y3.z;

	g_I3.at.x = g_Z3.x;
	g_I3.at.y = g_Z3.y;
	g_I3.at.z = g_Z3.z;

	g_I3.pos.x = g_O3.x;
	g_I3.pos.y = g_O3.y;
	g_I3.pos.z = g_O3.z;
}

void xMat3x3GetEuler(const xMat3x3* m, xVec3* a)
{
	float32 pitch;
	float32 yaw;
	float32 roll;

	pitch = -xasin(m->at.y);

	if (pitch < (0.5f * PI))
	{
		if (pitch > (-0.5f * PI))
		{
			yaw = xatan2(m->at.x, m->at.z);
			roll = xatan2(m->right.y, m->up.y);
		}
		else
		{
			yaw = -xatan2(-m->up.x, m->right.x);
			roll = 0.0f;
		}
	}
	else
	{
		yaw = xatan2(-m->up.x, m->right.x);
		roll = 0.0f;
	}

	a->x = yaw;
	a->y = pitch;
	a->z = roll;
}

void xMat4x3MoveLocalRight(xMat4x3* m, float32 mag)
{
	m->pos.x += m->right.x * mag;
	m->pos.y += m->right.y * mag;
	m->pos.z += m->right.z * mag;
}

void xMat4x3MoveLocalUp(xMat4x3* m, float32 mag)
{
	m->pos.x += m->up.x * mag;
	m->pos.y += m->up.y * mag;
	m->pos.z += m->up.z * mag;
}

void xMat4x3MoveLocalAt(xMat4x3* m, float32 mag)
{
	m->pos.x += m->at.x * mag;
	m->pos.y += m->at.y * mag;
	m->pos.z += m->at.z * mag;
}

void xMat3x3Euler(xMat3x3* m, const xVec3* ypr)
{
	xMat3x3Euler(m, ypr->x, ypr->y, ypr->z);
}

void xMat3x3Euler(xMat3x3* m, float32 yaw, float32 pitch, float32 roll)
{
	float32 f27 = isin(yaw);
	float32 f28 = icos(yaw);
	float32 f29 = isin(pitch);
	float32 f30 = icos(pitch);
	float32 f31 = isin(roll);
	float32 f1 = icos(roll);

	m->right.x = f28 * f1 + f31 * (f27 * f29);
	m->right.y = f30 * f31;
	m->right.z = -f27 * f1 + f31 * (f28 * f29);

	m->up.x = -f28 * f31 + f1 * (f27 * f29);
	m->up.y = f30 * f1;
	m->up.z = f27 * f31 + f1 * (f28 * f29);

	m->at.x = f27 * f30;
	m->at.y = -f29;
	m->at.z = f28 * f30;

	m->flags = 0;
}

NONMATCH void xQuatFromMat(xQuat* q, const xMat3x3* m)
{
	static int32 nxt[3] = { 1, 2, 0 };

	const float32* mp = (const float32*)m;
	float32* qvp = (float32*)q;
	float32 tr;
	float32 root;
	int32 i;
	int32 j;
	int32 k;

	tr = m->right.x + m->up.y + m->at.z;

	if (tr > 0.0f)
	{
		root = xsqrt(1.0f + tr);

		q->s = 0.5f * root;
		q->v.x = 0.5f / root * (m->at.y - m->up.z);
		q->v.y = 0.5f / root * (m->right.z - m->at.x);
		q->v.z = 0.5f / root * (m->up.x - m->right.y);
	}
	else
	{
		i = 0;

		if (m->up.y > m->right.x)
		{
			i = 1;
		}

		if (m->at.z > mp[i * 5])
		{
			i = 2;
		}

		j = nxt[i];
		k = nxt[j];

		tr = mp[i * 5] - mp[j * 5] - mp[k * 5];
		root = xsqrt(1.0f + tr);

		if (iabs(root) < 0.00001f)
		{
			xQuatCopy(q, &g_IQ);
		}
		else
		{
			qvp[i] = 0.5f * root;
			q->s = 0.5f / root * (mp[j + k * 4] - mp[k + j * 4]);
			qvp[j] = 0.5f / root * (mp[i + j * 4] + mp[j + i * 4]);
			qvp[k] = 0.5f / root * (mp[i + k * 4] + mp[k + i * 4]);

			if (q->s < 0.0f)
			{
				xQuatFlip(q, q);
			}
		}
	}
}

void xQuatToMat(const xQuat* q, xMat3x3* m)
{
	float32 tx = 2.0f * q->v.x;
	float32 ty = 2.0f * q->v.y;
	float32 tz = 2.0f * q->v.z;
	float32 tsx = tx * q->s;
	float32 tsy = ty * q->s;
	float32 tsz = tz * q->s;
	float32 txx = tx * q->v.x;
	float32 txy = ty * q->v.x;
	float32 txz = tz * q->v.x;
	float32 tyy = ty * q->v.y;
	float32 tyz = tz * q->v.y;
	float32 tzz = tz * q->v.z;

	m->right.x = 1.0f - tyy - tzz;
	m->right.y = txy - tsz;
	m->right.z = txz + tsy;

	m->up.x = txy + tsz;
	m->up.y = 1.0f - tzz - txx;
	m->up.z = tyz - tsx;

	m->at.x = txz - tsy;
	m->at.y = tyz + tsx;
	m->at.z = 1.0f - txx - tyy;

	m->flags = 0;
}

void xQuatToAxisAngle(const xQuat* q, xVec3* a, float32* t)
{
	*t = 2.0f * xacos(q->s);

	xVec3Normalize(a, &q->v);
}

float32 xQuatNormalize(xQuat* o, const xQuat* q)
{
	float32 len2 = xQuatLength2(q);

	if (len2 == 1.0f)
	{
		if (o != q)
		{
			xQuatCopy(o, q);
		}

		return 1.0f;
	}

	if (len2 == 0.0f)
	{
		if (o != q)
		{
			xQuatCopy(o, q);
		}

		return 0.0f;
	}

	float32 len = xsqrt(len2);
	float32 one_len = 1.0f / len;

	xQuatSMul(o, q, one_len);

	return len;
}

void xQuatMul(xQuat* o, const xQuat* a, const xQuat* b)
{
	o->v.x = a->v.x * b->s + a->s * b->v.x + a->v.y * b->v.z - a->v.z * b->v.y;
	o->v.y = a->v.y * b->s + a->s * b->v.y + a->v.z * b->v.x - a->v.x * b->v.z;
	o->v.z = a->v.z * b->s + a->s * b->v.z + a->v.x * b->v.y - a->v.y * b->v.x;
	o->s = a->s * b->s - a->v.x * b->v.x - a->v.y * b->v.y - a->v.z * b->v.z;

	xQuatNormalize(o, o);
}

void xQuatDiff(xQuat* o, const xQuat* a, const xQuat* b)
{
	xQuatConj(o, a);
	xQuatMul(o, o, b);

	if (o->s < 0.0f)
	{
		xQuatFlip(o, o);
	}
}