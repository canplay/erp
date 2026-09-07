export interface Response<T> {
  status: number;
  data?: T;
  message: string;
}

export interface User {
  id: string;
  username: string;
  primaryEmail: string;
  primaryPhone: string;
  name: string;
  avatar: string;
  customData: object;
  identities: {
    additionalProperty1: {
      userId: string;
      details: object;
    };
    additionalProperty2: {
      userId: string;
      details: object;
    };
  };
  lastSignInAt: 42.0;
  createdAt: 42.0;
  updatedAt: 42.0;
  profile: {
    familyName: string;
    givenName: string;
    middleName: string;
    nickname: string;
    preferredUsername: string;
    profile: string;
    website: string;
    gender: string;
    birthdate: string;
    zoneinfo: string;
    locale: string;
    address: {
      formatted: string;
      streetAddress: string;
      locality: string;
      region: string;
      postalCode: string;
      country: string;
    };
  };
  applicationId: string;
  isSuspended: true;
  hasPassword: true;
  ssoIdentities: [
    {
      tenantId: string;
      id: string;
      userId: string;
      issuer: string;
      identityId: string;
      detail: object;
      createdAt: 42.0;
      updatedAt: 42.0;
      ssoConnectorId: string;
    },
  ];
}
